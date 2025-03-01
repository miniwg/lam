<?php

namespace LAM\UPLOAD;

use LamTemporaryFilesManager;
use ZipArchive;
use accountContainer;

/*

 This code is part of LDAP Account Manager (http://www.ldap-account-manager.org/)
 Copyright (C) 2016 - 2024  Roland Gruber

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 */

/**
 * Creates LDAP accounts for file upload.
 *
 * @author Roland Gruber
 * @package tools
 */

/** LDAP handle */
include_once('ldap.inc');
/** PDF */
include_once('pdf.inc');

/**
 * Creates LDAP accounts for file upload.
 *
 * @author Roland Gruber
 * @package tools
 */
class Uploader {

	private $accounts;
	private $data;
	private $type;
	private $endTime;

	private const TIME_LIMIT = 10;

	public static function cleanSession() {
		foreach ($_SESSION as $key => &$value) {
			if (str_starts_with($key, 'mass_')) {
				unset($_SESSION[$key]);
			}
		}
	}

	/**
	 * Constructor
	 *
	 * @param \LAM\TYPES\ConfiguredType $type account type
	 */
	public function __construct($type) {
		$this->accounts = json_decode(lamDecrypt($_SESSION['mass_accounts']), true);
		$this->data = json_decode(lamDecrypt($_SESSION['mass_data']), true);
		$this->type = $type;
		$startTime = time();
		$maxTime = get_cfg_var('max_execution_time') - 5;
		if ($maxTime > self::TIME_LIMIT) {
			$maxTime = self::TIME_LIMIT;
		}
		if ($maxTime <= 0) {
			$maxTime = self::TIME_LIMIT;
		}
		$this->endTime = $startTime + $maxTime;
	}

	/**
	 * Start or continues the file upload.
	 *
	 * @return String JSON of upload status
	 */
	public function doUpload() {
		$this->securityCheck();
		if (!isset($_SESSION['mass_counter'])) {
			return $this->startUpload();
		}
		elseif ($_SESSION['mass_counter'] < sizeof($this->accounts)) {
			return $this->continueUpload();
		}
		elseif (!isset($_SESSION['mass_postActions']['finished'])) {
			return $this->runPostActions();
		}
		elseif (($_SESSION['mass_pdf']['structure'] != null) && !isset($_SESSION['mass_pdf']['finished'])) {
			return $this->createPDF();
		}
		return $this->buildUploadStatus(100, true, '', 100, true, 100);
	}

	/**
	 * Continues to upload accounts.
	 *
	 * @return String JSON of upload status
	 */
	private function continueUpload() {
		while (($_SESSION['mass_counter'] < sizeof($this->accounts)) && ($this->endTime > time())) {
			$this->uploadEntry($_SESSION['mass_counter']);
			$_SESSION['mass_counter']++;
		}
		$accountsProgress = round(($_SESSION['mass_counter'] * 100) / sizeof($this->accounts));
		$accountsFinished = ($_SESSION['mass_counter'] == sizeof($this->accounts));
		return $this->buildUploadStatus($accountsProgress, $accountsFinished);
	}

	/**
	 * Performs the upload of a single LDAP entry.
	 *
	 * @param int $position position in $this->accounts.
	 */
	private function uploadEntry($position) {
		$attrs = $this->accounts[$position];
		$dn = $attrs['dn'];
		unset($attrs['dn']);
		// remove informational attributes
		foreach ($attrs as $key => $value) {
			if (str_starts_with($key, 'INFO.')) {
				unset($attrs[$key]);
			}
		}
		// run preactions
		$preAttributes = [];
		foreach ($attrs as $key => $value) {
			$preAttributes[$key] = &$attrs[$key];
		}
		$preAttributes['dn'] = &$dn;
		$preMessages = doUploadPreActions($this->type, $_SESSION['mass_selectedModules'], $preAttributes);
		$preActionOk = true;
		for ($i = 0; $i < sizeof($preMessages); $i++) {
			if (($preMessages[$i][0] == 'ERROR') || ($preMessages[$i][0] == 'WARN')) {
				$preActionOk = false;
				$_SESSION['mass_errors'][] = $preMessages[$i];
			}
		}
		if ($preActionOk) {
			$errors = $this->deleteOldEntryIfNeeded($dn, $this->accounts[$position]);
			if (!empty($errors)) {
				$_SESSION['mass_errors'] = array_merge($_SESSION['mass_errors'], $errors);
				$_SESSION['mass_failed'][] = $position;
				return;
			}
			// add LDAP entry
			$success = @ldap_add($_SESSION['ldap']->server(), $dn, $attrs);
			if (!$success) {
				$errorMessage = [
					"ERROR",
					_("LAM was unable to create account %s! An LDAP error occurred."),
					getDefaultLDAPErrorString($_SESSION['ldap']->server()),
					[$position]];
				$_SESSION['mass_errors'][] = $errorMessage;
				$_SESSION['mass_failed'][] = $position;
			}
		}
	}

	/**
	 * Deletes an existing entry if overwrite option is used.
	 *
	 * @param string $dn DN
	 * @param array $attributes attribute list
	 * @return array list of error messages
	 */
	private function deleteOldEntryIfNeeded($dn, $attributes) {
		if (isset($attributes['INFO.overwrite']) && ($attributes['INFO.overwrite'] === 'true')) {
			$attrs = ldapGetDN($dn, ['dn']);
			if ($attrs !== null) {
				return deleteDN($dn, true);
			}
		}
		return [];
	}

	/**
	 * Starts the file upload.
	 *
	 * @return String JSON of upload status
	 */
	private function startUpload() {
		$_SESSION['mass_counter'] = 0;
		return $this->buildUploadStatus();
	}

	/**
	 * Returns the upload status as JSON.
	 *
	 * @param number $accountsProgress progress for LDAP entries
	 * @param string $accountsFinished all entries in LDAP
	 * @param string $postActionsTitle title for current post action
	 * @param number $postActionsProgress progress for post actions
	 * @param string $postActionsFinished post actions finished
	 * @param number $pdfProgress PDF creation progress
	 * @return String JSON status
	 */
	private function buildUploadStatus($accountsProgress = 0, $accountsFinished = false,
									   $postActionsTitle = '', $postActionsProgress = 0, $postActionsFinished = false,
									   $pdfProgress = 0) {
		$pdfFinished = ($_SESSION['mass_pdf']['structure'] == null) || isset($_SESSION['mass_pdf']['finished']);
		$allDone = $accountsFinished && $postActionsFinished && $pdfFinished;
		$errorHtml = '';
		if ($allDone && !empty($_SESSION['mass_errors'])) {
			foreach ($_SESSION['mass_errors'] as $error) {
				$text = $error[2] ?? '';
				$vars = $error[3] ?? [];
				$errorHtml .= StatusMessage($error[0], $error[1], $text, $vars, true);
			}
		}
		$status = [
			'title' => _("LDAP upload in progress. Please wait."),
			'titleFinished' => _("Upload has finished"),
			'titleErrors' => _("There were errors while uploading:"),
			'titlePDF' => _('Create PDF files'),
			'accountsProgress' => $accountsProgress,
			'accountsFinished' => $accountsFinished,
			'postActionsTitle' => $postActionsTitle,
			'postActionsProgress' => $postActionsProgress,
			'postActionsFinished' => $postActionsFinished,
			'pdfProgress' => $pdfProgress,
			'pdfFinished' => $pdfFinished,
			'allDone' => $allDone,
			'errorHtml' => $errorHtml,
			'typeId' => $this->type->getId()
		];
		return json_encode($status);
	}

	/**
	 * Checks for security violations and stops processing if needed.
	 */
	private function securityCheck() {
		if (!isLoggedIn() || empty($this->type)
			|| $this->type->isHidden()
			|| !checkIfNewEntriesAreAllowed($this->type->getId())
			|| !checkIfWriteAccessIsAllowed($this->type->getId())) {
			die;
		}
	}

	/**
	 * Performs any post create actions by modules.
	 *
	 * @return String JSON of upload status
	 */
	private function runPostActions() {
		$return = $this->runModulePostActions();
		while (!isset($_SESSION['mass_postActions']['finished']) && ($this->endTime > time())) {
			$return = $this->runModulePostActions();
		}
		$title = _("Additional tasks for module:") . ' ' . getModuleAlias($return['module'], $this->type->getScope());
		$progress = round($return['progress']);
		$finished = isset($_SESSION['mass_postActions']['finished']);
		return $this->buildUploadStatus(100, true, $title, $progress, $finished);
	}

	/**
	 * Runs a single post create action by modules.
	 *
	 * @return array status array
	 */
	private function runModulePostActions() {
		$return = doUploadPostActions($this->type, $this->data, $_SESSION['mass_ids'], $_SESSION['mass_failed'], $_SESSION['mass_selectedModules'], $this->accounts);
		if ($return['status'] == 'finished') {
			$_SESSION['mass_postActions']['finished'] = true;
		}
		if (isset($return['errors'])) {
			for ($i = 0; $i < sizeof($return['errors']); $i++) {
				$_SESSION['mass_errors'][] = $return['errors'][$i];
			}
		}
		return $return;
	}

	/**
	 * Creates the PDF files.
	 *
	 * @return String JSON of upload status
	 */
	private function createPDF() {
		$file = $_SESSION['mass_pdf']['file'];
		$pdfStructure = $_SESSION['mass_pdf']['structure'];
		$font = $_SESSION['mass_pdf']['font'];
		$pdfZip = new ZipArchive();
		$tempFilesManager = new LamTemporaryFilesManager();
		if ($_SESSION['mass_pdf']['counter'] == 0) {
			$pdfZipResult = @$pdfZip->open($tempFilesManager->getFileSystemPath($file), ZipArchive::OVERWRITE);
			if (!$pdfZipResult === true) {
				$_SESSION['mass_errors'][] = ['ERROR', _('Unable to create ZIP file for PDF export.')];
				$_SESSION['mass_pdf']['finished'] = true;
			}
		}
		else {
			@$pdfZip->open($tempFilesManager->getFileSystemPath($file));
		}
		while (!isset($_SESSION['mass_pdf']['finished']) && ($this->endTime > time())) {
			$attrs = $this->accounts[$_SESSION['mass_pdf']['counter']];
			$dn = $attrs['dn'];
			// get informational attributes
			$infoAttributes = [];
			foreach ($attrs as $key => $value) {
				if (str_starts_with($key, 'INFO.')) {
					$infoAttributes[$key] = $value;
				}
			}
			// load account
			$_SESSION['mass_pdfAccount'] = new accountContainer($this->type, 'mass_pdfAccount');
			$pdfErrors = $_SESSION['mass_pdfAccount']->load_account($dn, $infoAttributes);
			if (sizeof($pdfErrors) > 0) {
				$_SESSION['mass_errors'] = array_merge($_SESSION['mass_errors'], $pdfErrors);
				$_SESSION['mass_pdf']['finished'] = true;
				break;
			}
			// create and save PDF
			$pdfContent = \LAM\PDF\createModulePDF([$_SESSION['mass_pdfAccount']], $pdfStructure, $font, true);
			$fileName = $dn . '.pdf';
			$pdfZip->addFromString($fileName, $pdfContent);
			$_SESSION['mass_pdf']['counter']++;
			if ($_SESSION['mass_pdf']['counter'] >= sizeof($this->accounts)) {
				$_SESSION['mass_pdf']['finished'] = true;
			}
		}
		@$pdfZip->close();
		$progress = round(($_SESSION['mass_pdf']['counter'] * 100) / sizeof($this->accounts));
		return $this->buildUploadStatus(100, true, '', 100, true, $progress);
	}

}
