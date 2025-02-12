<?php
/*

  This code is part of LDAP Account Manager (http://www.ldap-account-manager.org/)
  Copyright (C) 2005 - 2024  Roland Gruber

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
 * Manages Samba 3 domain entries.
 *
 * @package modules
 * @author Roland Gruber
 */

use LAM\TYPES\ConfiguredType;

/**
 * Manages Samba 3 domain entries.
 *
 * @package modules
 */
class sambaDomain extends baseModule {

	/**
	 * These attributes will be ignored by default if a new account is copied from an existing one.
	 */
	private const ATTRIBUTES_TO_IGNORE_ON_COPY = ['sambaDomainName', 'sambaSID', 'sambaNextRid',
		'sambaNextGroupRid', 'sambaNextUserRid'];

	/**
	 * Returns true if this module can manage accounts of the current type, otherwise false.
	 *
	 * @return boolean true if module fits
	 */
	public function can_manage() {
		return in_array($this->get_scope(), ['smbDomain']);
	}

	/**
	 * Returns meta data that is interpreted by parent class
	 *
	 * @return array array with meta data
	 *
	 * @see baseModule::get_metaData()
	 */
	function get_metaData() {
		$return = [];
		// icon
		$return['icon'] = 'samba.svg';
		// alias name
		$return["alias"] = _("Samba domain");
		// this is a base module
		$return["is_base"] = true;
		// RDN attribute
		$return["RDN"] = ["sambaDomainName" => "high"];
		// LDAP filter
		$return["ldap_filter"] = ['or' => "(objectClass=sambaDomain)"];
		// module dependencies
		$return['dependencies'] = ['depends' => [], 'conflicts' => []];
		// managed object classes
		$return['objectClasses'] = ['sambaDomain'];
		// managed attributes
		$return['attributes'] = ['sambaDomainName', 'sambaSID', 'sambaNextRid', 'sambaNextGroupRid',
			'sambaNextUserRid', 'sambaAlgorithmicRidBase', 'sambaMinPwdLength', 'sambaPwdHistoryLength',
			'sambaLogonToChgPwd', 'sambaForceLogoff', 'sambaRefuseMachinePwdChange', 'sambaLockoutThreshold',
			'sambaMinPwdAge', 'sambaMaxPwdAge', 'sambaLockoutDuration', 'sambaLockoutObservationWindow'];
		// help Entries
		$return['help'] = [
			'sambaDomainName' => [
				"Headline" => _("Domain name"), 'attr' => 'sambaDomainName',
				"Text" => _("The name of your Windows domain or workgroup.")
			],
			'sambaSID' => [
				"Headline" => _("Domain SID"), 'attr' => 'sambaSID',
				"Text" => _("The SID of your Samba server. Get it with \"net getlocalsid\".")
			],
			'nextRID' => [
				"Headline" => _("Next RID"), 'attr' => 'sambaNextRid',
				"Text" => _("Next RID to use when creating accounts (only used by Winbind).")
			],
			'nextUserRID' => [
				"Headline" => _("Next user RID"), 'attr' => 'sambaNextUserRid',
				"Text" => _("Next RID to use when creating user accounts (only used by Winbind).")
			],
			'nextGroupRID' => [
				"Headline" => _("Next group RID"), 'attr' => 'sambaNextGroupRid',
				"Text" => _("Next RID to use when creating group accounts (only used by Winbind).")
			],
			'RIDbase' => [
				"Headline" => _("RID base"), 'attr' => 'sambaAlgorithmicRidBase',
				"Text" => _("Used for calculating RIDs from UID/GID. Do not change if unsure.")
			],
			'minPwdLength' => [
				"Headline" => _("Minimal password length"), 'attr' => 'sambaMinPwdLength',
				"Text" => _("Here you can specify the minimum number of characters for a user password.")
			],
			'pwdHistLength' => [
				"Headline" => _("Password history length"), 'attr' => 'sambaPwdHistoryLength',
				"Text" => _("This is the number of passwords which are saved to prevent that users reuse old passwords.")
			],
			'logonToChgPwd' => [
				"Headline" => _("Logon for password change"), 'attr' => 'sambaLogonToChgPwd',
				"Text" => _("If set then users need to login to change their password.")
			],
			'forceLogoff' => [
				"Headline" => _("Disconnect users outside logon hours"), 'attr' => 'sambaForceLogoff',
				"Text" => _("Disconnects users if they are logged in outside logon hours.")
			],
			'refuseMachinePwdChange' => [
				"Headline" => _("Allow machine password changes"), 'attr' => 'sambaRefuseMachinePwdChange',
				"Text" => _("Defines if workstations may change their passwords.")
			],
			'lockoutThreshold' => [
				"Headline" => _("Lockout users after bad logon attempts"), 'attr' => 'sambaLockoutThreshold',
				"Text" => _("This is the number of bad logon attempts (0 - 999) before the account is deactivated. 0 means unlimited attempts.")
			],
			'minPwdAge' => [
				"Headline" => _("Minimum password age"), 'attr' => 'sambaMinPwdAge',
				"Text" => _("Number of seconds after the user is allowed to change his password again.")
			],
			'maxPwdAge' => [
				"Headline" => _("Maximum password age"), 'attr' => 'sambaMaxPwdAge',
				"Text" => _("Number of seconds after which the user must change his password.")
			],
			'lockoutDuration' => [
				"Headline" => _("Lockout duration"), 'attr' => 'sambaLockoutDuration',
				"Text" => _("This is the time (in minutes) for which the user may not log in after the account was locked. -1 means forever.")
			],
			'lockoutObservationWindow' => [
				"Headline" => _("Reset time after lockout"), 'attr' => 'sambaLockoutObservationWindow',
				"Text" => _("Number of minutes after which the bad logon attempts are reset.")
			]];
		// upload fields
		$return['upload_columns'] = [
			[
				'name' => 'sambaDomain_domainName',
				'description' => _('Domain name'),
				'help' => 'sambaDomainName',
				'example' => _('Workgroup'),
				'required' => true
			],
			[
				'name' => 'sambaDomain_domainSID',
				'description' => _('Domain SID'),
				'help' => 'sambaSID',
				'example' => 'S-1-1-22-123-123-123',
				'required' => true
			],
			[
				'name' => 'sambaDomain_RIDbase',
				'description' => _('RID base'),
				'help' => 'RIDbase',
				'example' => '1000',
				'default' => 1000
			],
			[
				'name' => 'sambaDomain_nextRID',
				'description' => _('Next RID'),
				'help' => 'nextRID',
				'example' => '12345'
			],
			[
				'name' => 'sambaDomain_nextUserRID',
				'description' => _('Next user RID'),
				'help' => 'nextUserRID',
				'example' => '12345'
			],
			[
				'name' => 'sambaDomain_nextGroupRID',
				'description' => _('Next group RID'),
				'help' => 'nextGroupRID',
				'example' => '12345'
			]
		];
		// available PDF fields
		$return['PDF_fields'] = [
			'domainName' => _('Domain name'),
			'domainSID' => _('Domain SID'),
			'nextRID' => _('Next RID'),
			'nextUserRID' => _('Next user RID'),
			'nextGroupRID' => _('Next group RID'),
			'RIDbase' => _('RID base'),
			'minPwdLength' => _('Minimal password length'),
			'pwdHistoryLength' => _('Password history length'),
			'logonToChgPwd' => _('Logon for password change'),
			'forceLogoff' => _('Disconnect users outside logon hours'),
			'refuseMachinePwdChange' => _('Allow machine password changes'),
			'lockoutThreshold' => _('Lockout users after bad logon attempts'),
			'minPwdAge' => _('Minimum password age'),
			'maxPwdAge' => _('Maximum password age'),
			'lockoutDuration' => _('Lockout duration'),
			'lockoutObservationWindow' => _('Reset time after lockout')
		];
		return $return;
	}

	/**
	 * This function fills the error message array with messages
	 */
	function load_Messages() {
		$this->messages['domainName'][0] = ['ERROR', _('Domain name is invalid!')];
		$this->messages['domainName'][1] = ['ERROR', _('Account %s:') . ' sambaDomain_domainName', _('Domain name is invalid!')];
		$this->messages['domainSID'][0] = ['ERROR', _('Samba 3 domain SID is invalid!')];
		$this->messages['domainSID'][1] = ['ERROR', _('Account %s:') . ' sambaDomain_domainSID', _('Samba 3 domain SID is invalid!')];
		$this->messages['nextRID'][0] = ['ERROR', _('Next RID is not a number!')];
		$this->messages['nextRID'][1] = ['ERROR', _('Account %s:') . ' sambaDomain_nextRID', _('Next RID is not a number!')];
		$this->messages['nextUserRID'][0] = ['ERROR', _('Next user RID is not a number!')];
		$this->messages['nextUserRID'][1] = ['ERROR', _('Account %s:') . ' sambaDomain_nextUserRID', _('Next user RID is not a number!')];
		$this->messages['nextGroupRID'][0] = ['ERROR', _('Next group RID is not a number!')];
		$this->messages['nextGroupRID'][1] = ['ERROR', _('Account %s:') . ' sambaDomain_nextGroupRID', _('Next group RID is not a number!')];
		$this->messages['RIDbase'][0] = ['ERROR', _('Algorithmic RID base is not a number!')];
		$this->messages['RIDbase'][1] = ['ERROR', _('Account %s:') . ' sambaDomain_RIDbase', _('Algorithmic RID base is not a number!')];
		$this->messages['pwdAge_cmp'][0] = ['ERROR', _('Maximum password age'), _('Password maximum age must be bigger than password minimum age.')];
		$this->messages['pwdAgeMin'][0] = ['ERROR', _('Minimum password age'), _('Password minimum age must be are natural number.')];
		$this->messages['pwdAgeMax'][0] = ['ERROR', _('Maximum password age'), _('Password maximum age must be are natural number.')];
		$this->messages['lockoutDuration'][0] = ['ERROR', _('Lockout duration'), _('Lockout duration must be are natural number.')];
		$this->messages['lockoutObservationWindow'][0] = ['ERROR', _('Reset time after lockout'), _('Reset time after lockout must be are natural number.')];
		$this->messages['lockoutThreshold'][0] = ['ERROR', _('Lockout users after bad logon attempts'), _('Lockout users after bad logon attempts must be between 0 and 999.')];
	}

	/**
	 * {@inheritDoc}
	 */
	public function loadAttributesFromAccountCopy(array $ldapAttributes, array $attributesToIgnore = []): void {
		$attributesToIgnore = array_merge(baseModule::ATTRIBUTES_TO_IGNORE_ON_COPY_DEFAULT, self::ATTRIBUTES_TO_IGNORE_ON_COPY);
		parent::loadAttributesFromAccountCopy($ldapAttributes, $attributesToIgnore);
	}

	/**
	 * Returns the HTML meta data for the main account page.
	 *
	 * @return htmlElement HTML meta data
	 */
	function display_html_attributes() {
		$return = new htmlResponsiveRow();
		// domain name
		if ($this->getAccountContainer()->isNewAccount) {
			$this->addSimpleInputTextField($return, 'sambaDomainName', _('Domain name'), true);
		}
		else {
			$return->addLabel(new htmlOutputText(_('Domain name')));
			$domainNameGroup = new htmlGroup();
			$domainName = $this->attributes['sambaDomainName'][0];
			$domainNameGroup->addElement(new htmlOutputText($domainName));
			$domainNameGroup->addElement(new htmlHelpLink('sambaDomainName'));
			$return->addField($domainNameGroup);
		}
		// domain SID
		if ($this->getAccountContainer()->isNewAccount) {
			$this->addSimpleInputTextField($return, 'sambaSID', _('Domain SID'), true);
		}
		else {
			$return->addLabel(new htmlOutputText(_('Domain SID')));
			$domainSidGroup = new htmlGroup();
			$domainSID = $this->attributes['sambaSID'][0];
			$domainSidGroup->addElement(new htmlOutputText($domainSID));
			$domainSidGroup->addElement(new htmlHelpLink('sambaSID'));
			$return->addField($domainSidGroup);
		}

		$return->add(new htmlSubTitle(_("Password policy")), 12);

		/* group policies */

		// minimum password length
		$sambaMinPwdLength = '-';
		if (isset($this->attributes['sambaMinPwdLength'][0])) {
			$sambaMinPwdLength = $this->attributes['sambaMinPwdLength'][0];
		}
		$return->add(new htmlResponsiveSelect('minPwdLength', ['-', 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15],
			[$sambaMinPwdLength], _('Minimal password length'), 'minPwdLength'), 12);
		// password history length
		$sambaPwdHistoryLength = '-';
		if (isset($this->attributes['sambaPwdHistoryLength'][0])) {
			$sambaPwdHistoryLength = $this->attributes['sambaPwdHistoryLength'][0];
		}
		$return->add(new htmlResponsiveSelect('pwdHistLength', ['-', 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15],
			[$sambaPwdHistoryLength], _('Password history length'), 'pwdHistLength'), 12);
		// logon to change password
		$sambaLogonToChgPwd = '-';
		if (isset($this->attributes['sambaLogonToChgPwd'][0])) {
			$sambaLogonToChgPwd = $this->attributes['sambaLogonToChgPwd'][0];
		}
		$logonPwdChangeSelect = new htmlResponsiveSelect('logonToChgPwd', ['-' => '-', _('Off') => '0', _('On') => '2'],
			[$sambaLogonToChgPwd], _('Logon for password change'), 'logonToChgPwd');
		$logonPwdChangeSelect->setHasDescriptiveElements(true);
		$return->add($logonPwdChangeSelect);
		// force logoff
		$sambaForceLogoff = '-';
		if (isset($this->attributes['sambaForceLogoff'][0])) {
			$sambaForceLogoff = $this->attributes['sambaForceLogoff'][0];
		}
		$forceLogoffSelect = new htmlResponsiveSelect('forceLogoff', ['-' => '-', _('Off') => '-1', _('On') => '0'],
			[$sambaForceLogoff], _('Disconnect users outside logon hours'), 'forceLogoff');
		$forceLogoffSelect->setHasDescriptiveElements(true);
		$return->add($forceLogoffSelect);
		// do not allow machine password change
		$sambaRefuseMachinePwdChange = '-';
		if (isset($this->attributes['sambaRefuseMachinePwdChange'][0])) {
			$sambaRefuseMachinePwdChange = $this->attributes['sambaRefuseMachinePwdChange'][0];
		}
		$refuseMachPwdChange = new htmlResponsiveSelect('refuseMachinePwdChange', ['-' => '-', _('Off') => '0', _('On') => '1'],
			[$sambaRefuseMachinePwdChange], _('Allow machine password changes'), 'refuseMachinePwdChange');
		$refuseMachPwdChange->setHasDescriptiveElements(true);
		$return->add($refuseMachPwdChange);
		// Lockout users after bad logon attempts
		$sambaLockoutThreshold = '';
		if (isset($this->attributes['sambaLockoutThreshold'][0])) {
			$sambaLockoutThreshold = $this->attributes['sambaLockoutThreshold'][0];
		}
		$sambaLockoutThresholdInput = new htmlResponsiveInputField(_('Lockout users after bad logon attempts'), 'lockoutThreshold', $sambaLockoutThreshold, 'lockoutThreshold');
		$sambaLockoutThresholdInput->setType('number');;
		$return->add($sambaLockoutThresholdInput);
		// Minimum password age
		$sambaMinPwdAge = '';
		if (isset($this->attributes['sambaMinPwdAge'][0])) {
			$sambaMinPwdAge = $this->attributes['sambaMinPwdAge'][0];
		}
		$sambaMinPwdAgeInput = new htmlResponsiveInputField(_('Minimum password age'), 'minPwdAge', $sambaMinPwdAge, 'minPwdAge');
		$sambaMinPwdAgeInput->setType('number');;
		$return->add($sambaMinPwdAgeInput);
		// Maximum password age
		$sambaMaxPwdAge = '';
		if (isset($this->attributes['sambaMaxPwdAge'][0])) {
			$sambaMaxPwdAge = $this->attributes['sambaMaxPwdAge'][0];
		}
		$sambaMaxPwdAgeInput = new htmlResponsiveInputField(_('Maximum password age'), 'maxPwdAge', $sambaMaxPwdAge, 'maxPwdAge');
		$sambaMaxPwdAgeInput->setValidationPattern('[-]?[0-9]+');
		$return->add($sambaMaxPwdAgeInput);
		// Lockout duration
		$sambaLockoutDuration = '';
		if (isset($this->attributes['sambaLockoutDuration'][0])) {
			$sambaLockoutDuration = $this->attributes['sambaLockoutDuration'][0];
		}
		$sambaLockoutDurationInput = new htmlResponsiveInputField(_('Lockout duration'), 'lockoutDuration', $sambaLockoutDuration, 'lockoutDuration');
		$sambaLockoutDurationInput->setType('number');
		$return->add($sambaLockoutDurationInput);
		// Reset time after lockout
		$sambaLockoutObservationWindow = '';
		if (isset($this->attributes['sambaLockoutObservationWindow'][0])) {
			$sambaLockoutObservationWindow = $this->attributes['sambaLockoutObservationWindow'][0];
		}
		$sambaLockoutObservationWindowInput = new htmlResponsiveInputField(_('Reset time after lockout'), 'lockoutObservationWindow', $sambaLockoutObservationWindow, 'lockoutObservationWindow');
		$sambaLockoutObservationWindowInput->setType('number');
		$return->add($sambaLockoutObservationWindowInput);

		$return->add(new htmlSubTitle(_('RID settings')));

		/* RID settings */

		// next RID
		$nextRID = '';
		if (isset($this->attributes['sambaNextRid'][0])) {
			$nextRID = $this->attributes['sambaNextRid'][0];
		}
		$nextRIDInput = new htmlResponsiveInputField(_('Next RID'), 'nextRID', $nextRID, 'nextRID');
		$nextRIDInput->setMinimumAndMaximumNumber();
		$return->add($nextRIDInput, 12);
		// next user RID
		$nextUserRID = '';
		if (isset($this->attributes['sambaNextUserRid'][0])) {
			$nextUserRID = $this->attributes['sambaNextUserRid'][0];
		}
		$nextUserRIDInput = new htmlResponsiveInputField(_('Next user RID'), 'nextUserRID', $nextUserRID, 'nextUserRID');
		$nextUserRIDInput->setMinimumAndMaximumNumber();
		$return->add($nextUserRIDInput, 12);
		// next group RID
		$nextGroupRID = '';
		if (isset($this->attributes['sambaNextGroupRid'][0])) {
			$nextGroupRID = $this->attributes['sambaNextGroupRid'][0];
		}
		$nextGroupRIDInput = new htmlResponsiveInputField(_('Next group RID'), 'nextGroupRID', $nextGroupRID, 'nextGroupRID');
		$nextGroupRIDInput->setMinimumAndMaximumNumber();
		$return->add($nextGroupRIDInput, 12);
		// RID base
		if (!isset($this->attributes['sambaAlgorithmicRidBase'][0])) {
			$this->attributes['sambaAlgorithmicRidBase'][0] = 1000;
		}
		if ($this->getAccountContainer()->isNewAccount) {
			$ridBaseInput = new htmlResponsiveInputField(_('RID base'), 'RIDbase', $this->attributes['sambaAlgorithmicRidBase'][0], 'RIDbase');
			$ridBaseInput->setRequired(true);
			$ridBaseInput->setMinimumAndMaximumNumber();
			$return->add($ridBaseInput, 12);
		}
		else {
			$return->addLabel(new htmlOutputText(_('RID base')));
			$ridBaseGroup = new htmlGroup();
			$ridBaseGroup->addElement(new htmlOutputText($this->attributes['sambaAlgorithmicRidBase'][0]));
			$ridBaseGroup->addElement(new htmlHelpLink('RIDbase'));
			$return->addField($ridBaseGroup);
		}
		return $return;
	}

	/**
	 * Processes user input of the primary module page.
	 * It checks if all input values are correct and updates the associated LDAP attributes.
	 *
	 * @return array list of info/error messages
	 */
	function process_attributes() {
		$errors = [];
		if ($this->getAccountContainer()->isNewAccount) {
			// domain SID
			$this->attributes['sambaSID'][0] = $_POST['sambaSID'];
			if (!get_preg($_POST['sambaSID'], 'domainSID')) {
				$errors[] = $this->messages['domainSID'][0];
			}
			else {
				$this->attributes['sambaSID'][0] = $_POST['sambaSID'];
			}
			// RID base
			if (!get_preg($_POST['RIDbase'], 'digit') && !($_POST['RIDbase'] == '')) {
				$errors[] = $this->messages['RIDbase'][0];
			}
			else {
				$this->attributes['sambaAlgorithmicRidBase'][0] = $_POST['RIDbase'];
			}
			// domain name
			if (!get_preg($_POST['sambaDomainName'], 'domainname') && !($_POST['sambaDomainName'] == '')) {
				$errors[] = $this->messages['domainName'][0];
			}
			else {
				$this->attributes['sambaDomainName'][0] = $_POST['sambaDomainName'];
			}
		}
		// next RID
		if (!get_preg($_POST['nextRID'], 'digit')) {
			$errors[] = $this->messages['nextRID'][0];
		}
		else {
			$this->attributes['sambaNextRid'][0] = $_POST['nextRID'];
		}
		// next user RID
		if (!get_preg($_POST['nextUserRID'], 'digit')) {
			$errors[] = $this->messages['nextUserRID'][0];
		}
		else {
			$this->attributes['sambaNextUserRid'][0] = $_POST['nextUserRID'];
		}
		// next group RID
		if (!get_preg($_POST['nextGroupRID'], 'digit')) {
			$errors[] = $this->messages['nextGroupRID'][0];
		}
		else {
			$this->attributes['sambaNextGroupRid'][0] = $_POST['nextGroupRID'];
		}
		// minimum password length
		if ($_POST['minPwdLength'] === '-') {
			if (isset($this->attributes['sambaMinPwdLength'])) {
				unset($this->attributes['sambaMinPwdLength'][0]);
			}
		}
		else {
			$this->attributes['sambaMinPwdLength'][0] = $_POST['minPwdLength'];
		}
		// password history length
		if ($_POST['pwdHistLength'] === '-') {
			if (isset($this->attributes['sambaPwdHistoryLength'])) {
				unset($this->attributes['sambaPwdHistoryLength'][0]);
			}
		}
		else {
			$this->attributes['sambaPwdHistoryLength'][0] = $_POST['pwdHistLength'];
		}
		// logon for password change
		if ($_POST['logonToChgPwd'] === '-') {
			if (isset($this->attributes['sambaLogonToChgPwd'])) {
				unset($this->attributes['sambaLogonToChgPwd'][0]);
			}
		}
		else {
			$this->attributes['sambaLogonToChgPwd'][0] = $_POST['logonToChgPwd'];
		}
		// force logoff
		if ($_POST['forceLogoff'] === '-') {
			if (isset($this->attributes['sambaForceLogoff'])) {
				unset($this->attributes['sambaForceLogoff'][0]);
			}
		}
		else {
			$this->attributes['sambaForceLogoff'][0] = $_POST['forceLogoff'];
		}
		// do not allow machine password changes
		if ($_POST['refuseMachinePwdChange'] === '-') {
			if (isset($this->attributes['sambaRefuseMachinePwdChange'])) {
				unset($this->attributes['sambaRefuseMachinePwdChange'][0]);
			}
		}
		else {
			$this->attributes['sambaRefuseMachinePwdChange'][0] = $_POST['refuseMachinePwdChange'];
		}
		// Lockout users after bad logon attempts
		if (!isset($_POST['lockoutThreshold']) || ($_POST['lockoutThreshold'] == '')) {
			if (isset($this->attributes['sambaLockoutThreshold'])) {
				unset($this->attributes['sambaLockoutThreshold'][0]);
			}
		}
		else {
			if (is_numeric($_POST['lockoutThreshold']) && ($_POST['lockoutThreshold'] >= 0) && ($_POST['lockoutThreshold'] < 1000)) {
				$this->attributes['sambaLockoutThreshold'][0] = $_POST['lockoutThreshold'];
			}
			else {
				$errors[] = $this->messages['lockoutThreshold'][0];
			}
		}
		// Minimum password age
		if (!isset($_POST['minPwdAge']) || ($_POST['minPwdAge'] == '')) {
			if (isset($this->attributes['sambaMinPwdAge'])) {
				unset($this->attributes['sambaMinPwdAge'][0]);
			}
		}
		else {
			if (is_numeric($_POST['minPwdAge']) && ($_POST['minPwdAge'] > -2)) {
				$this->attributes['sambaMinPwdAge'][0] = $_POST['minPwdAge'];
			}
			else {
				$errors[] = $this->messages['pwdAgeMin'][0];
			}
		}
		// Maximum password age
		if (!isset($_POST['maxPwdAge']) || ($_POST['maxPwdAge'] == '')) {
			if (isset($this->attributes['sambaMaxPwdAge'])) {
				unset($this->attributes['sambaMaxPwdAge'][0]);
			}
		}
		else {
			if (!is_numeric($_POST['maxPwdAge']) || ($_POST['maxPwdAge'] < -1)) {
				$errors[] = $this->messages['pwdAgeMax'][0];
			}
			elseif (($_POST['maxPwdAge'] > 1) && ($_POST['maxPwdAge'] < $_POST['minPwdAge'])) {
				$errors[] = $this->messages['pwdAge_cmp'][0];
			}
			else {
				$this->attributes['sambaMaxPwdAge'][0] = $_POST['maxPwdAge'];
			}
		}
		// Lockout duration
		if (!isset($_POST['lockoutDuration']) || ($_POST['lockoutDuration'] == '')) {
			if (isset($this->attributes['sambaLockoutDuration'])) {
				unset($this->attributes['sambaLockoutDuration'][0]);
			}
		}
		else {
			if (is_numeric($_POST['lockoutDuration']) && ($_POST['lockoutDuration'] > -2)) {
				$this->attributes['sambaLockoutDuration'][0] = $_POST['lockoutDuration'];
			}
			else {
				$errors[] = $this->messages['lockoutDuration'][0];
			}
		}
		// Reset time after lockout
		if (!isset($_POST['lockoutObservationWindow']) || ($_POST['lockoutObservationWindow'] == '')) {
			if (isset($this->attributes['sambaLockoutObservationWindow'])) {
				unset($this->attributes['sambaLockoutObservationWindow'][0]);
			}
		}
		else {
			if (is_numeric($_POST['lockoutObservationWindow']) && ($_POST['lockoutObservationWindow'] > -1)) {
				$this->attributes['sambaLockoutObservationWindow'][0] = $_POST['lockoutObservationWindow'];
			}
			else {
				$errors[] = $this->messages['lockoutObservationWindow'][0];
			}
		}

		return $errors;
	}

	/**
	 * {@inheritDoc}
	 * @see baseModule::build_uploadAccounts()
	 */
	function build_uploadAccounts($rawAccounts, $ids, &$partialAccounts, $selectedModules, &$type) {
		$messages = [];
		for ($i = 0; $i < sizeof($rawAccounts); $i++) {
			// add object class
			if (!in_array("sambaDomain", $partialAccounts[$i]['objectClass'])) {
				$partialAccounts[$i]['objectClass'][] = "sambaDomain";
			}
			// domain name
			$this->mapSimpleUploadField($rawAccounts, $ids, $partialAccounts, $i, 'sambaDomain_domainName', 'sambaDomainName',
				'domainname', $this->messages['domainName'][1], $messages);
			// domain SID
			$this->mapSimpleUploadField($rawAccounts, $ids, $partialAccounts, $i, 'sambaDomain_domainSID', 'sambaSID',
				'domainSID', $this->messages['domainSID'][1], $messages);
			// RID base
			$partialAccounts[$i]['sambaAlgorithmicRidBase'] = '1000';
			$this->mapSimpleUploadField($rawAccounts, $ids, $partialAccounts, $i, 'sambaDomain_RIDbase', 'sambaAlgorithmicRidBase',
				'digit', $this->messages['RIDbase'][1], $messages);
			// next RID
			$this->mapSimpleUploadField($rawAccounts, $ids, $partialAccounts, $i, 'sambaDomain_nextRID', 'sambaNextRid',
				'digit', $this->messages['nextRID'][1], $messages);
			// next user RID
			$this->mapSimpleUploadField($rawAccounts, $ids, $partialAccounts, $i, 'sambaDomain_nextUserRID', 'sambaNextUserRid',
				'digit', $this->messages['nextUserRID'][1], $messages);
			// next group RID
			$this->mapSimpleUploadField($rawAccounts, $ids, $partialAccounts, $i, 'sambaDomain_nextGroupRID', 'sambaNextGroupRid',
				'digit', $this->messages['nextGroupRID'][1], $messages);
		}
		return $messages;
	}

	/**
	 * {@inheritDoc}
	 * @see baseModule::get_pdfEntries()
	 */
	function get_pdfEntries($pdfKeys, $typeId) {
		$return = [];
		$this->addSimplePDFField($return, 'domainName', _('Domain name'), 'sambaDomainName');
		$this->addSimplePDFField($return, 'domainSID', _('Domain SID'), 'sambaSID');
		$this->addSimplePDFField($return, 'nextRID', _('Next RID'), 'sambaNextRid');
		$this->addSimplePDFField($return, 'nextUserRID', _('Next user RID'), 'sambaNextUserRid');
		$this->addSimplePDFField($return, 'nextGroupRID', _('Next group RID'), 'sambaNextGroupRid');
		$this->addSimplePDFField($return, 'RIDbase', _('RID base'), 'sambaAlgorithmicRidBase');
		$this->addSimplePDFField($return, 'minPwdLength', _('Minimal password length'), 'sambaMinPwdLength');
		$this->addSimplePDFField($return, 'pwdHistoryLength', _('Password history length'), 'sambaPwdHistoryLength');
		$this->addSimplePDFField($return, 'lockoutThreshold', _('Lockout users after bad logon attempts'), 'sambaLockoutThreshold');
		$this->addSimplePDFField($return, 'minPwdAge', _('Minimum password age'), 'sambaMinPwdAge');
		$this->addSimplePDFField($return, 'maxPwdAge', _('Maximum password age'), 'sambaMaxPwdAge');
		$this->addSimplePDFField($return, 'lockoutDuration', _('Lockout duration'), 'sambaLockoutDuration');
		$this->addSimplePDFField($return, 'lockoutObservationWindow', _('Reset time after lockout'), 'sambaLockoutObservationWindow');

		if (isset($this->attributes['sambaLogonToChgPwd'])) {
			$logonToChgPwd = _('Off');
			if ($this->attributes['sambaPwdHistoryLength'][0] == 2) {
				$logonToChgPwd = _('On');
			}
			$this->addPDFKeyValue($return, 'logonToChgPwd', _('Logon for password change'), $logonToChgPwd);
		}
		if (isset($this->attributes['sambaForceLogoff'])) {
			$forceLogoff = _('Off');
			if ($this->attributes['sambaForceLogoff'][0] == 0) {
				$forceLogoff = _('On');
			}
			$this->addPDFKeyValue($return, 'forceLogoff', _('Disconnect users outside logon hours'), $forceLogoff);
		}
		if (isset($this->attributes['sambaRefuseMachinePwdChange'])) {
			$refuseMachinePwdChange = _('Off');
			if ($this->attributes['sambaRefuseMachinePwdChange'][0] == 0) {
				$refuseMachinePwdChange = _('On');
			}
			$this->addPDFKeyValue($return, 'refuseMachinePwdChange', _('Allow machine password changes'), $refuseMachinePwdChange);
		}
		return $return;
	}

	/**
	 * @inheritDoc
	 */
	public function getListAttributeDescriptions(ConfiguredType $type): array {
		return [
			"sambasid" => _("Domain SID"),
			"sambadomainname" => _("Domain name")
		];
	}

}
