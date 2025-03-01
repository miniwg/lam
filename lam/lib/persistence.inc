<?php

namespace LAM\PERSISTENCE;

use LAM\DB\CronDatabase;
use LAM\LOGIN\WEBAUTHN\WebauthnManager;
use LAM\PDF\PDFStructure;
use LAM\PDF\PdfStructurePersistenceManager;
use LAM\PROFILES\AccountProfilePersistenceManager;
use LAMCfgMain;
use LAMConfig;
use LAMException;
use PDO;
use PDOException;
use SelfServicePersistence;
use selfServiceProfile;
use ServerProfilePersistenceManager;
use function LAM\PDF\getPDFStructures;

/*

  This code is part of LDAP Account Manager (http://www.ldap-account-manager.org/)
  Copyright (C) 2020 - 2024  Roland Gruber

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
 * This file includes functions to manage the persistence of LAM's configuration files.
 *
 * @package configuration
 * @author Roland Gruber
 */

include_once __DIR__ . '/config.inc';
include_once __DIR__ . '/profiles.inc';

/**
 * Exporter for LAM's configuration data.
 */
class ConfigDataExporter {

	/**
	 * Exports LAM's configuration data in JSON format.
	 * @throws LAMException error during export
	 */
	public function exportAsJson() {
		$mainCfg = $this->_getMainConfiguration();
		$jsonData = [];
		$jsonData['mainConfig'] = $this->_getMainConfigData($mainCfg);
		$jsonData['certificates'] = $this->_getCertificates($mainCfg);
		$serverProfilesPersistenceManager = new ServerProfilePersistenceManager();
		$serverProfileNames = $serverProfilesPersistenceManager->getProfiles();
		$serverProfiles = [];
		foreach ($serverProfileNames as $serverProfileName) {
			$serverProfiles[$serverProfileName] = $serverProfilesPersistenceManager->loadProfile($serverProfileName);
		}
		$jsonData['serverProfiles'] = $this->_getServerProfiles($serverProfiles);
		$jsonData['accountProfiles'] = $this->_getAccountProfiles($serverProfiles);
		$jsonData['accountProfileTemplates'] = $this->_getAccountProfileTemplates();
		$jsonData['pdfProfiles'] = $this->_getPdfProfiles($serverProfiles);
		$jsonData['pdfProfileTemplates'] = $this->_getPdfProfileTemplates();
		$jsonData['selfServiceProfiles'] = $this->_getSelfServiceProfiles();
		$jsonData['webauthn'] = $this->_getWebauthn();
		$jsonData['cronJobs'] = $this->_getCronJobData($serverProfiles);
		$jsonData['requestAccess'] = $this->_getRequestAccessData();
		return json_encode($jsonData);
	}

	/**
	 * Returns the main configuration.
	 *
	 * @return LAMCfgMain main config
	 */
	public function _getMainConfiguration() {
		return new LAMCfgMain();
	}

	/**
	 * Internal function to read master configuration.
	 *
	 * @param LAMCfgMain $mainCfg main config
	 * @return array data
	 */
	public function _getMainConfigData($mainCfg) {
		return $mainCfg->exportData(true);
	}

	/**
	 * Returns the certificate file content.
	 *
	 * @param LAMCfgMain $mainCfg main config
	 * @return array data
	 */
	public function _getCertificates($mainCfg) {
		return $mainCfg->exportCertificates();
	}

	/**
	 * Returns the content of the server profiles.
	 *
	 * @param array $serverProfiles list of server profiles (name => object)
	 * @return array $data
	 */
	public function _getServerProfiles($serverProfiles) {
		$data = [];
		foreach ($serverProfiles as $profileName => $serverProfile) {
			$data[$profileName] = $serverProfile->exportData();
		}
		return $data;
	}

	/**
	 * Returns the content of the account profiles.
	 *
	 * @param array $serverProfiles list of server profiles (name => object)
	 * @return array $data export data
	 * @throws LAMException error reading profiles
	 */
	public function _getAccountProfiles($serverProfiles) {
		$data = [];
		$accountProfilePersistenceManager = new AccountProfilePersistenceManager();
		foreach ($serverProfiles as $profileName => $serverProfile) {
			foreach ($serverProfile->get_ActiveTypes() as $typeId) {
				$accountProfileNames = $accountProfilePersistenceManager->getAccountProfileNames($typeId, $profileName);
				foreach ($accountProfileNames as $accountProfileName) {
					$accountProfile = $accountProfilePersistenceManager->loadAccountProfile($typeId, $accountProfileName, $profileName);
					$data[$profileName][$typeId][$accountProfileName] = $accountProfile;
				}
			}
		}
		return $data;
	}

	/**
	 * Returns the content of the account profile templates.
	 *
	 * @return array $data export data
	 * @throws LAMException error reading template
	 */
	public function _getAccountProfileTemplates() {
		$data = [];
		$accountProfilePersistenceManager = new AccountProfilePersistenceManager();
		$accountProfileTemplateNames = $accountProfilePersistenceManager->getAccountProfileTemplateNames();
		foreach ($accountProfileTemplateNames as $scope => $templateNames) {
			foreach ($templateNames as $templateName) {
				$accountProfileTemplate = $accountProfilePersistenceManager->loadAccountProfileTemplate($scope, $templateName);
				$data[$scope][$templateName] = $accountProfileTemplate;
			}
		}
		return $data;
	}

	/**
	 * Returns the content of the PDF profiles.
	 *
	 * @param array $serverProfiles list of server profiles (name => object)
	 * @return array $data
	 * @throws LAMException error reading logo
	 */
	public function _getPdfProfiles($serverProfiles): array {
		$data = [];
		$pdfStructurePersistenceManager = new PdfStructurePersistenceManager();
		foreach ($serverProfiles as $profileName => $serverProfile) {
			foreach ($serverProfile->get_ActiveTypes() as $typeId) {
				$pdfProfileNames = $pdfStructurePersistenceManager->getPDFStructures($profileName, $typeId);
				foreach ($pdfProfileNames as $pdfProfileName) {
					$pdfStructure = $pdfStructurePersistenceManager->readPdfStructure($profileName, $typeId, $pdfProfileName);
					$data[$profileName]['structures'][$typeId][$pdfProfileName] = $pdfStructure->export();
				}
			}
			$logoData = $pdfStructurePersistenceManager->getPdfLogos($profileName);
			foreach ($logoData as $logo) {
				$logoName = $logo->getName();
				$logoBinary = $pdfStructurePersistenceManager->getPdfLogoBinary($profileName, $logoName);
				$data[$profileName]['logos'][$logoName] = base64_encode($logoBinary);
			}
		}
		return $data;
	}

	/**
	 * Returns the content of the account profile templates.
	 *
	 * @return array $data
	 * @throws LAMException error reading template
	 */
	public function _getPdfProfileTemplates() {
		$data = [];
		$pdfStructurePersistenceManager = new PdfStructurePersistenceManager();
		$pdfTemplateNames = $pdfStructurePersistenceManager->getPdfStructureTemplateNames();
		foreach ($pdfTemplateNames as $scope => $templateNames) {
			foreach ($templateNames as $templateName) {
				$pdfStructure = $pdfStructurePersistenceManager->readPdfStructureTemplate($scope, $templateName);
				$data['structures'][$scope][$templateName] = $pdfStructure->export();
			}
		}
		$logoNames = $pdfStructurePersistenceManager->getPdfTemplateLogoNames();
		foreach ($logoNames as $logoName) {
			$data['logos'][$logoName] = base64_encode($pdfStructurePersistenceManager->getPdfTemplateLogoBinary($logoName));
		}
		return $data;
	}

	/**
	 * Returns the content of the self service profiles.
	 *
	 * @return array data
	 */
	public function _getSelfServiceProfiles() {
		$data = [];
		$selfServicePersistence = new SelfServicePersistence();
		$profileTypes = $selfServicePersistence->getProfiles();
		$selfServicePersistence = new SelfServicePersistence();
		foreach ($profileTypes as $profileType => $profileNames) {
			foreach ($profileNames as $profileName) {
				try {
					$profile = $selfServicePersistence->load($profileName, $profileType);
				}
				catch (LAMException $e) {
					logNewMessage(LOG_ERR, $e->getTitle() . ' ' . $profileName);
					continue;
				}
				$data[$profileType][$profileName] = $profile->export();
			}
		}
		return $data;
	}

	/**
	 * Returns the content of the webauthn database.
	 *
	 * @return array data
	 */
	public function _getWebauthn() {
		$data = [];
		if (extension_loaded('PDO')
			&& in_array('sqlite', \PDO::getAvailableDrivers())) {
			include_once __DIR__ . '/webauthn.inc';
			$webauthnManager = new WebauthnManager();
			$webauthnDatabase = $webauthnManager->getDatabase();
			$data = $webauthnDatabase->export();
		}
		return $data;
	}

	/**
	 * Returns the content of the cron jobs.
	 *
	 * @param array $serverProfiles list of server profiles (name => object)
	 * @return array data
	 * @throws LAMException error reading cron jobs
	 */
	public function _getCronJobData(array $serverProfiles): array {
		$data = [];
		if (!isLAMProVersion()) {
			return $data;
		}
		include_once 'database.inc';
		foreach ($serverProfiles as $serverProfileName => $serverProfile) {
			try {
				$cronDatabase = new CronDatabase($serverProfile);
				if (empty($cronDatabase->getJobs())) {
					continue;
				}
				$cronDatabase->connect();
				foreach ($cronDatabase->getJobRuns() as $jobRun) {
					$data[$serverProfileName]['jobRuns'][] = [
						'runId' => $jobRun->getRunId(),
						'name' => $jobRun->getName(),
						'jobId' => $jobRun->getJobId(),
						'time' => $jobRun->getTime()->format('Y-m-d H:i:s'),
						'isOk' => $jobRun->isOk(),
						'message' => $jobRun->getMessages()
					];
				}
				$jobs = $cronDatabase->getJobs();
				foreach ($jobs as $jobId => $job) {
					$exportData = $job->export($cronDatabase->getPdo(), $jobId);
					if (!empty($exportData)) {
						$data[$serverProfileName]['jobData'][$jobId] = $exportData;
					}
				}
			}
			catch (LAMException $e) {
				logNewMessage(LOG_ERR, $e->getTitle());
			}
		}
		return $data;
	}

	/**
	 * Returns the content for request access requests.
	 *
	 * @return array data
	 * @throws LAMException error reading data
	 */
	public function _getRequestAccessData(): array {
		$data = [];
		if (!isLAMProVersion()) {
			return $data;
		}
		$database = \requestAccess::getDatabase();
		return $database->exportData();
	}

}

/**
 * Importer for LAM's configuration data.
 */
class ConfigDataImporter {

	/**
	 * Returns a list of possible import objects.
	 *
	 * @param string $json JSON data
	 * @return ImporterStep[] steps
	 * @throws LAMException if invalid format
	 */
	public function getPossibleImportSteps($json) {
		$data = json_decode($json, true);
		if ($data === null) {
			throw new LAMException(_('Unable to read import file.'));
		}
		$steps = [];
		foreach ($data as $key => $value) {
			switch ($key) {
				case 'mainConfig':
					$steps[] = new ImporterStep(_('General settings'), 'mainConfig', $value);
					break;
				case 'certificates':
					$steps[] = new ImporterStep(_('SSL certificates'), 'certificates', $value);
					break;
				case 'serverProfiles':
					$mainStep = new ImporterStep(_('Server profiles'), 'serverProfiles', $value);
					foreach ($value as $profileName => $profileData) {
						$mainStep->addSubStep(new ImporterStep($profileName, 'serverProfile_' . $profileName, $profileData));
					}
					$steps[] = $mainStep;
					break;
				case 'accountProfiles':
					$mainStep = new ImporterStep(_('Account profiles'), 'accountProfiles', $value);
					foreach ($value as $profileName => $profileData) {
						$mainStep->addSubStep(new ImporterStep($profileName, 'accountProfile_' . $profileName, $profileData));
					}
					$steps[] = $mainStep;
					break;
				case 'accountProfileTemplates':
					$steps[] = new ImporterStep(_('Account profiles') . ' - ' . _('Global templates'), 'accountProfileTemplates', $value);
					break;
				case 'pdfProfiles':
					$mainStep = new ImporterStep(_('PDF structures'), 'pdfProfiles', $value);
					foreach ($value as $profileName => $profileData) {
						$mainStep->addSubStep(new ImporterStep($profileName, 'pdfProfile_' . $profileName, $profileData));
					}
					$steps[] = $mainStep;
					break;
				case 'pdfProfileTemplates':
					$steps[] = new ImporterStep(_('PDF structures') . ' - ' . _('Global templates'), 'pdfProfileTemplates', $value);
					break;
				case 'selfServiceProfiles':
					$mainStep = new ImporterStep(_('Self service profiles'), 'selfServiceProfiles', $value);
					foreach ($value as $scope => $profiles) {
						foreach ($profiles as $profileName => $profileData) {
							$mainStep->addSubStep(new ImporterStep($profileName, 'selfServiceProfile_' . $scope . '__' . $profileName, $profileData));
						}
					}
					$steps[] = $mainStep;
					break;
				case 'webauthn':
					if (extension_loaded('PDO')
						&& in_array('sqlite', PDO::getAvailableDrivers())) {
						$steps[] = new ImporterStep(_('WebAuthn devices'), 'webauthn', $value);
					}
					break;
				case 'cronJobs':
					if (isLAMProVersion()) {
						$mainStep = new ImporterStep(_('Cron jobs'), 'cronJobs', $value);
						foreach ($value as $profileName => $profileData) {
							$mainStep->addSubStep(new ImporterStep($profileName, 'cronJob_' . $profileName, $profileData));
						}
						$steps[] = $mainStep;
					}
					break;
				case 'requestAccess':
					if (isLAMProVersion()) {
						$mainStep = new ImporterStep(_('Request access'), 'requestAccess', $value);
						if (isset($value['openRequests'])) {
							$mainStep->addSubStep(new ImporterStep(_('Open requests'), 'requestAccess_openRequests', $value['openRequests']));
						}
						if (isset($value['historicRequests'])) {
							$mainStep->addSubStep(new ImporterStep(_('Request history'), 'requestAccess_historicRequests', $value['historicRequests']));
						}
						$steps[] = $mainStep;
					}
					break;
				default:
					logNewMessage(LOG_WARNING, 'Unknown import type: ' . $key);
			}
		}
		if (empty($steps)) {
			throw new LAMException(_('Unable to read import file.'));
		}
		return $steps;
	}

	/**
	 * Runs the actual import.
	 *
	 * @param ImporterStep[] $steps import steps
	 * @throws LAMException if error occurred
	 */
	public function runImport($steps) {
		foreach ($steps as $step) {
			if (!$step->isActive()) {
				continue;
			}
			$key = $step->getKey();
			switch ($key) {
				case 'mainConfig':
					$this->importMainConfig($step->getValue());
					break;
				case 'certificates':
					$this->importCertificates($step->getValue());
					break;
				case 'serverProfiles':
					$this->importServerProfiles($step);
					break;
				case 'accountProfiles':
					$this->importAccountProfiles($step);
					break;
				case 'accountProfileTemplates':
					$this->importAccountProfileTemplates($step);
					break;
				case 'pdfProfiles':
					$this->importPdfProfiles($step);
					break;
				case 'pdfProfileTemplates':
					$this->importPdfProfileTemplates($step);
					break;
				case 'selfServiceProfiles':
					$this->importSelfServiceProfiles($step);
					break;
				case 'webauthn':
					$this->importWebauthn($step);
					break;
				case 'cronJobs':
					$this->importCronJobs($step);
					break;
				case 'requestAccess':
					$this->importRequestAccess($step);
					break;
				default:
					logNewMessage(LOG_WARNING, 'Unknown import type: ' . $key);
			}
		}
	}

	/**
	 * Imports the main configuration.
	 *
	 * @param array $data main config data
	 * @throws LAMException error during import
	 */
	private function importMainConfig($data): void {
		$cfgMain = new LAMCfgMain();
		$cfgMain->importData($data);
		$cfgMain->save();
	}

	/**
	 * Imports the SSL certificates.
	 *
	 * @param null|string $data file content
	 * @throws LAMException error during import
	 */
	private function importCertificates($data): void {
		$cfgMain = new LAMCfgMain();
		$cfgMain->importCertificates($data);
	}

	/**
	 * Imports the server profiles.
	 *
	 * @param ImporterStep $step step
	 * @throws LAMException error during import
	 */
	private function importServerProfiles(ImporterStep $step): void {
		$failedProfiles = [];
		$serverProfilePersistenceManager = new ServerProfilePersistenceManager();
		foreach ($step->getSubSteps() as $profileStep) {
			if (!$profileStep->isActive()) {
				continue;
			}
			$data = $profileStep->getValue();
			$profileName = str_replace('serverProfile_', '', $profileStep->getKey());
			try {
				$serverProfile = new LAMConfig();
				$serverProfile->setName($profileName);
				$serverProfile->importData($data);
				$serverProfilePersistenceManager->saveProfile($serverProfile, $profileName);
			}
			catch (LAMException $e) {
				logNewMessage(LOG_ERR, $e->getTitle());
				$failedProfiles[] = $profileName;
			}
		}
		if (!empty($failedProfiles)) {
			throw new LAMException(_('Unable to save server profile.'), implode(', ', $failedProfiles));
		}
	}

	/**
	 * Imports the account profiles.
	 *
	 * @param ImporterStep $step step
	 * @throws LAMException error during import
	 */
	private function importAccountProfiles(ImporterStep $step): void {
		$failedProfiles = [];
		$accountProfilePersistenceManager = new AccountProfilePersistenceManager();
		foreach ($step->getSubSteps() as $profileStep) {
			if (!$profileStep->isActive()) {
				continue;
			}
			$data = $profileStep->getValue();
			$serverProfileName = str_replace('accountProfile_', '', $profileStep->getKey());
			foreach ($data as $typeId => $accountProfiles) {
				foreach ($accountProfiles as $accountProfileName => $accountProfileData) {
					try {
						$accountProfilePersistenceManager->writeAccountProfile($typeId, $accountProfileName, $serverProfileName, $accountProfileData);
					}
					catch (LAMException $e) {
						$failedProfiles[] = $serverProfileName . ':' . $typeId . ':' . $accountProfileName;
					}
				}
			}
		}
		if (!empty($failedProfiles)) {
			throw new LAMException(_('Unable to save account profile.'), implode(', ', $failedProfiles));
		}
	}

	/**
	 * Imports the account profile templates.
	 *
	 * @param ImporterStep $step step
	 * @throws LAMException error during import
	 */
	private function importAccountProfileTemplates(ImporterStep $step): void {
		$data = $step->getValue();
		$accountProfilePersistenceManager = new AccountProfilePersistenceManager();
		foreach ($data as $scope => $accountProfileTemplates) {
			foreach ($accountProfileTemplates as $accountProfileTemplateName => $accountProfileData) {
				$accountProfilePersistenceManager->writeAccountProfileTemplate($scope, $accountProfileTemplateName, $accountProfileData);
			}
		}
	}

	/**
	 * Imports the PDF profiles.
	 *
	 * @param ImporterStep $step step
	 * @throws LAMException error during import
	 */
	private function importPdfProfiles(ImporterStep $step): void {
		$failedProfiles = [];
		$pdfStructurePersistenceManager = new PdfStructurePersistenceManager();
		foreach ($step->getSubSteps() as $profileStep) {
			if (!$profileStep->isActive()) {
				continue;
			}
			$data = $profileStep->getValue();
			$serverProfileName = str_replace('pdfProfile_', '', $profileStep->getKey());
			if (isset($data['structures'])) {
				foreach ($data['structures'] as $typeId => $pdfProfiles) {
					foreach ($pdfProfiles as $pdfProfileName => $pdfProfileData) {
						$structure = new PDFStructure();
						$structure->import($pdfProfileData);
						try {
							$pdfStructurePersistenceManager->savePdfStructure($serverProfileName, $typeId, $pdfProfileName, $structure);
						}
						catch (LAMException $e) {
							logNewMessage(LOG_ERR, $e->getTitle() . ' ' . $e->getMessage());
							$failedProfiles[] = $serverProfileName . ':' . $typeId . ':' . $pdfProfileName;
						}
					}
				}
			}
			if (isset($data['logos'])) {
				foreach ($data['logos'] as $logoFileName => $logoData) {
					$logoBinary = base64_decode($logoData);
					$pdfStructurePersistenceManager->savePdfLogo($serverProfileName, $logoFileName, $logoBinary);
				}
			}
		}
		if (!empty($failedProfiles)) {
			throw new LAMException(_('Could not save PDF structure, access denied.'), implode(', ', $failedProfiles));
		}
	}

	/**
	 * Imports the PDF profile templates.
	 *
	 * @param ImporterStep $step step
	 * @throws LAMException error during import
	 */
	private function importPdfProfileTemplates(ImporterStep $step): void {
		$pdfStructurePersistenceManager = new PdfStructurePersistenceManager();
		$failedNames = [];
		$data = $step->getValue();
		if (isset($data['structures'])) {
			foreach ($data['structures'] as $scope => $pdfProfiles) {
				foreach ($pdfProfiles as $pdfProfileName => $pdfProfileData) {
					$structure = new PDFStructure();
					$structure->import($pdfProfileData);
					try {
						$pdfStructurePersistenceManager->savePdfStructureTemplate($scope, $pdfProfileName, $structure);
					}
					catch (LAMException $e) {
						$failedNames[] = $scope . ':' . $pdfProfileName;
						logNewMessage(LOG_ERR, $e->getTitle() . ' ' . $e->getMessage());
					}
				}
			}
		}
		$failedLogos = [];
		if (isset($data['logos'])) {
			foreach ($data['logos'] as $logoFileName => $logoData) {
				$logoBinary = base64_decode($logoData);
				try {
					$pdfStructurePersistenceManager->savePdfTemplateLogo($logoFileName, $logoBinary);
				}
				catch (LAMException $e) {
					logNewMessage(LOG_ERR, $e->getTitle());
					$failedLogos[] = $logoFileName;
				}
			}
		}
		if (!empty($failedNames)) {
			throw new LAMException(_('Could not save PDF structure, access denied.'), implode(', ', $failedNames));
		}
		if (!empty($failedLogos)) {
			throw new LAMException(_('Unable to upload logo file.'), implode(', ', $failedLogos));
		}
	}

	/**
	 * Imports the self service profiles.
	 *
	 * @param ImporterStep $step importer step
	 * @throws LAMException error saving profiles
	 */
	private function importSelfServiceProfiles(ImporterStep $step): void {
		$failedProfiles = [];
		$selfServicePersistence = new SelfServicePersistence();
		foreach ($step->getSubSteps() as $profileStep) {
			if (!$profileStep->isActive()) {
				continue;
			}
			$data = $profileStep->getValue();
			$profileKey = str_replace('selfServiceProfile_', '', $profileStep->getKey());
			$profileKeyParts = explode('__', $profileKey);
			$profileScope = $profileKeyParts[0];
			$profileName = substr($profileKey, strlen($profileScope) + 2);
			try {
				$profile = selfServiceProfile::import($data);
				$selfServicePersistence->save($profileName, $profileScope, $profile);
			}
			catch (LAMException $e) {
				logNewMessage(LOG_ERR, $e->getTitle());
				$failedProfiles[] = $profileName . '.' . $profileScope;
			}
		}
		if (!empty($failedProfiles)) {
			throw new LAMException(_('Unable to save self service profile.'), implode(', ', $failedProfiles));
		}
	}

	/**
	 * Imports the webauthn data.
	 *
	 * @param ImporterStep $step importer step
	 * @throws LAMException error saving profiles
	 */
	private function importWebauthn(ImporterStep $step): void {
		$data = $step->getValue();
		include_once __DIR__ . '/webauthn.inc';
		$webauthnManager = new WebauthnManager();
		$webauthnDatabase = $webauthnManager->getDatabase();
		$webauthnDatabase->import($data);
	}

	/**
	 * Imports the cron jobs.
	 *
	 * @param ImporterStep $step step
	 * @throws LAMException error during import
	 */
	private function importCronJobs(ImporterStep $step): void {
		if (!isLAMProVersion()) {
			return;
		}
		$failedProfiles = [];
		include_once 'database.inc';
		$serverProfilePersistenceManager = new ServerProfilePersistenceManager();
		foreach ($step->getSubSteps() as $profileStep) {
			if (!$profileStep->isActive()) {
				continue;
			}
			$data = $profileStep->getValue();
			$serverProfileName = str_replace('cronJob_', '', $profileStep->getKey());
			$serverProfile = $serverProfilePersistenceManager->loadProfile($serverProfileName);
			$cronDatabase = new CronDatabase($serverProfile);
			$cronDatabase->connect();
			if (isset($data['jobRuns'])) {
				logNewMessage(LOG_DEBUG, 'Import ' . sizeof($data['jobRuns']) . ' cron job runs for ' . $serverProfileName);
				$cronDatabase->clearJobRuns();
				foreach ($data['jobRuns'] as $jobRun) {
					$cronDatabase->importJobRun($jobRun['runId'], $jobRun['name'], $jobRun['jobId'], $jobRun['time'], $jobRun['isOk'], $jobRun['message']);
				}
			}
			if (isset($data['jobData'])) {
				$jobs = $cronDatabase->getJobs();
				foreach ($data['jobData'] as $jobId => $jobData) {
					if (!isset($jobs[$jobId])) {
						continue;
					}
					$job = $jobs[$jobId];
					logNewMessage(LOG_DEBUG, 'Import ' . sizeof($jobData) . ' cron data entries for ' . $job->getName() . ' - ' . $jobId);
					$job->import($cronDatabase->getPdo(), $jobData, $jobId);
				}
			}
		}
		if (!empty($failedProfiles)) {
			throw new LAMException(_('Could not save PDF structure, access denied.'), implode(', ', $failedProfiles));
		}
	}

	/**
	 * Imports request access data.
	 *
	 * @param ImporterStep $step step
	 * @throws LAMException error accessing database
	 */
	private function importRequestAccess(ImporterStep $step): void {
		if (!isLAMProVersion()) {
			return;
		}
		$database = \requestAccess::getDatabase();
		foreach ($step->getSubSteps() as $subStep) {
			if (!$subStep->isActive()) {
				continue;
			}
			$data = $subStep->getValue();
			$stepName = str_replace('requestAccess_', '', $subStep->getKey());
			switch ($stepName) {
				case 'openRequests':
					$database->importOpenRequests($data);
					break;
				case 'historicRequests':
					$database->importRequestHistory($data);
					break;
			}
		}
	}

}

/**
 * Step of the import process.
 */
class ImporterStep {

	private $label;
	private $key;
	private $value;
	private $active = false;
	private $subSteps = [];

	/**
	 * Constructor.
	 *
	 * @param string $label label
	 * @param string $key key
	 * @param array $value value
	 */
	public function __construct($label, $key, $value) {
		$this->label = $label;
		$this->key = $key;
		$this->value = $value;
	}

	/**
	 * Returns the label.
	 *
	 * @return string label
	 */
	public function getLabel() {
		return $this->label;
	}

	/**
	 * Returns the key.
	 *
	 * @return string key
	 */
	public function getKey() {
		return $this->key;
	}

	/**
	 * Returns if this step should be executed.
	 *
	 * @return bool active
	 */
	public function isActive(): bool {
		return $this->active;
	}

	/**
	 * Sets if this step should be executed.
	 *
	 * @param bool $active active
	 */
	public function setActive(bool $active) {
		$this->active = $active;
	}

	/**
	 * Returns the value.
	 *
	 * @return string value
	 */
	public function getValue() {
		return $this->value;
	}

	/**
	 * Adds a sub-step.
	 *
	 * @param ImporterStep $subStep sub-step
	 */
	public function addSubStep($subStep) {
		$this->subSteps[] = $subStep;
	}

	/**
	 * Returns the sub-steps.
	 *
	 * @return ImporterStep[] sub-steps
	 */
	public function getSubSteps() {
		return $this->subSteps;
	}

}

/**
 * Manages the configuration persistence.
 *
 * @package LAM\PERSISTENCE
 */
class ConfigurationDatabase {

	/** table for schema versions */
	const TABLE_SCHEMA_VERSIONS = 'schema_versions';
	/** row name for system schema version */
	const ROW_VERSION_SYSTEM = 'system';
	/** table for main configuration */
	const TABLE_MAIN_CONFIG = 'main_configuration';
	/** row name for main config schema version */
	const ROW_VERSION_MAIN_CONFIG = 'main_configuration';

	private $cfgMain;

	/**
	 * @var PDO PDO
	 */
	private $pdo;

	/**
	 * Constructor
	 *
	 * @param LAMCfgMain $cfgMain main configuration
	 */
	public function __construct(LAMCfgMain $cfgMain) {
		$this->cfgMain = $cfgMain;
	}

	/**
	 * Returns if a remote database is to be used.
	 *
	 * @return bool use remote database
	 */
	public function useRemoteDb(): bool {
		return (!empty($this->cfgMain->configDatabaseType) && ($this->cfgMain->configDatabaseType === LAMCfgMain::DATABASE_MYSQL));
	}

	/**
	 * Checks if the database connection is ok.
	 *
	 * @throws PDOException error opening connection to database
	 */
	public function checkConnection() {
		if (!$this->useRemoteDb()) {
			return;
		}
		$pdo = $this->getPdo();
		$this->updateSchema($pdo);
	}

	/**
	 * Returns the PDO to read and write data.
	 *
	 * @return PDO PDO
	 * @throws PDOException error opening connection to database
	 */
	public function getPdo(): PDO {
		if ($this->pdo !== null) {
			return $this->pdo;
		}
		$url = $this->getPdoUrl();
		$this->pdo = new PDO($url, $this->cfgMain->configDatabaseUser, $this->cfgMain->configDatabasePassword, [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
		return $this->pdo;
	}

	/**
	 * Builds the PDO URL.
	 *
	 * @return string PDO URL
	 */
	private function getPdoUrl(): string {
		$portConf = empty($this->cfgMain->configDatabasePort) ? '' : ';port=' . $this->cfgMain->configDatabasePort;
		return 'mysql:host=' . $this->cfgMain->configDatabaseServer . $portConf . ';dbname=' . $this->cfgMain->configDatabaseName;
	}

	/**
	 * Updates the schema to latest version.
	 *
	 * @param PDO $pdo PDO
	 */
	private function updateSchema(PDO $pdo) {
		// initial system schema
		if (!dbTableExists($pdo, self::TABLE_SCHEMA_VERSIONS)) {
			$this->createInitialSchema($pdo);
		}

	}

	/**
	 * Creates the initial schema.
	 *
	 * @param PDO $pdo PDO
	 */
	private function createInitialSchema(PDO $pdo): void {
		logNewMessage(LOG_NOTICE, 'Create initial config database...');
		// schema versions
		$sql = 'create table ' . self::TABLE_SCHEMA_VERSIONS . '('
			. 'name VARCHAR(250) PRIMARY KEY,'
			. 'version INT(3) NOT NULL'
			. ');';
		$pdo->exec($sql);
		$sql = 'insert into ' . self::TABLE_SCHEMA_VERSIONS . ' (name, version) VALUES ("' . self::ROW_VERSION_SYSTEM . '", 1);';
		$pdo->exec($sql);
		// main configuration
		$sql = 'create table ' . self::TABLE_MAIN_CONFIG . '('
			. 'name VARCHAR(250) PRIMARY KEY,'
			. 'value LONGTEXT NOT NULL'
			. ');';
		$pdo->exec($sql);
		$sql = 'insert into ' . self::TABLE_SCHEMA_VERSIONS . ' (name, version) VALUES ("' . self::ROW_VERSION_MAIN_CONFIG . '", 1);';
		$pdo->exec($sql);
		logNewMessage(LOG_NOTICE, 'Created initial config database.');
	}

}

/**
 * Returns if the given table name exists
 *
 * @param PDO $pdo PDO
 * @param string $tableName table name
 * @return bool true if existing
 */
function dbTableExists(PDO $pdo, string $tableName): bool {
	try {
		$result = $pdo->query("SELECT 1 FROM $tableName LIMIT 1");
		return ($result === false) ? false : true;
	}
	catch (PDOException $e) {
		return false;
	}
}
