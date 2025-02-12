<?php
/*

  This code is part of LDAP Account Manager (http://www.ldap-account-manager.org/)
  Copyright (C) 2011 - 2024  Roland Gruber

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
 * Manages the hosts to which a user may login.
 *
 * @package modules
 * @author Roland Gruber
 */

use LAM\TYPES\ConfiguredType;

/**
 * Manages the hosts to which a user may login.
 *
 * @package modules
 */
class hostObject extends baseModule {

	/**
	 * Creates a new hostObject object.
	 *
	 * @param string $scope account type (user, group, host)
	 */
	function __construct($scope) {
		// call parent constructor
		parent::__construct($scope);
		$this->autoAddObjectClasses = false;
	}

	/**
	 * Returns true if this module can manage accounts of the current type, otherwise false.
	 *
	 * @return boolean true if module fits
	 */
	public function can_manage() {
		return in_array($this->get_scope(), ['user', 'group']);
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
		$return['icon'] = 'computer.svg';
		// alias name
		$return["alias"] = _("Hosts");
		// module dependencies
		$return['dependencies'] = ['depends' => [], 'conflicts' => []];
		// managed object classes
		$return['objectClasses'] = ['hostObject'];
		// managed attributes
		$return['attributes'] = ['host'];
		// help Entries
		$return['help'] = [
			'host' => [
				"Headline" => _("Hosts"), 'attr' => 'host',
				"Text" => _("Here you can specify the list of host names where this account has login privileges. The wildcard \"*\" represents all hosts. You may also use \"!\" in front of a host name to deny access to a host.")
			],
			'hostList' => [
				"Headline" => _("Hosts"), 'attr' => 'host',
				"Text" => _("Here you can specify the list of host names where this account has login privileges. The wildcard \"*\" represents all hosts. You may also use \"!\" in front of a host name to deny access to a host.")
					. ' ' . _("Multiple values are separated by comma.")
			],
			'autoAdd' => [
				"Headline" => _("Automatically add this extension"),
				"Text" => _("This will enable the extension automatically if this profile is loaded.")
			]
		];
		// upload fields
		$return['upload_columns'] = [
			[
				'name' => 'hostObject_hosts',
				'description' => _('Host list'),
				'help' => 'hostList',
				'example' => _('pc01,pc02')
			]
		];
		// available PDF fields
		$return['PDF_fields'] = [
			'hosts' => _('Host list')
		];
		$profileContainer = new htmlResponsiveRow();
		$profileContainer->add(new htmlResponsiveInputCheckbox('hostObject_addExt', false, _('Automatically add this extension'), 'autoAdd'));
		$profileContainer->add(new htmlResponsiveInputField(_('Host'), 'hostObject_host', null, 'hostList'));
		$return['profile_options'] = $profileContainer;
		return $return;
	}

	/**
	 * This function fills the error message array with messages
	 */
	function load_Messages() {
		$this->messages['host'][0] = ['ERROR', 'Please enter a valid host name.'];  // third array value is set dynamically
		$this->messages['host'][1] = ['ERROR', _('Account %s:') . ' hostObject_hosts', _('Please enter a valid list of host names.')];
	}

	/**
	 * Returns a list of modifications which have to be made to the LDAP account.
	 *
	 * @return array list of modifications
	 * <br>This function returns an array with 3 entries:
	 * <br>array( DN1 ('add' => array($attr), 'remove' => array($attr), 'modify' => array($attr)), DN2 .... )
	 * <br>DN is the DN to change. It may be possible to change several DNs (e.g. create a new user and add him to some groups via attribute memberUid)
	 * <br>"add" are attributes which have to be added to LDAP entry
	 * <br>"remove" are attributes which have to be removed from LDAP entry
	 * <br>"modify" are attributes which have to been modified in LDAP entry
	 * <br>"info" are values with informational value (e.g. to be used later by pre/postModify actions)
	 */
	function save_attributes() {
		if (!in_array('hostObject', $this->attributes['objectClass']) && !in_array('hostObject', $this->orig['objectClass'])) {
			// skip saving if the extension was not added/modified
			return [];
		}
		return parent::save_attributes();
	}

	/**
	 * Returns the HTML meta data for the main account page.
	 *
	 * @return htmlElement HTML meta data
	 */
	function display_html_attributes() {
		if (isset($_POST['addObjectClass'])) {
			$this->attributes['objectClass'][] = 'hostObject';
		}
		$return = new htmlResponsiveRow();
		if (in_array('hostObject', $this->attributes['objectClass'])) {
			$this->addMultiValueInputTextField($return, 'host', _('Host'));
			$return->addVerticalSpacer('2rem');
			$remButton = new htmlButton('remObjectClass', _('Remove host extension'));
			$remButton->setCSSClasses(['lam-danger']);
			$return->add($remButton, 12, 12, 12, 'text-center');
		}
		else {
			$return->add(new htmlButton('addObjectClass', _('Add host extension')));
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
		if (isset($_POST['remObjectClass'])) {
			$this->attributes['objectClass'] = array_delete(['hostObject'], $this->attributes['objectClass']);
			if (isset($this->attributes['host'])) {
				unset($this->attributes['host']);
			}
			return [];
		}
		if (!in_array('hostObject', $this->attributes['objectClass'])) {
			return [];
		}
		$errors = [];
		$this->processMultiValueInputTextField('host', $errors, 'hostObject');
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
			if (!in_array("hostObject", $partialAccounts[$i]['objectClass'])) {
				$partialAccounts[$i]['objectClass'][] = "hostObject";
			}
			// add hosts
			$this->mapSimpleUploadField($rawAccounts, $ids, $partialAccounts, $i, 'hostObject_hosts', 'host', 'hostObject', $this->messages['host'][1], $messages, '/,[ ]*/');
		}
		return $messages;
	}

	/**
	 * {@inheritDoc}
	 * @see baseModule::get_pdfEntries()
	 */
	function get_pdfEntries($pdfKeys, $typeId) {
		$return = [];
		$this->addSimplePDFField($return, 'hosts', _('Host list'), 'host');
		return $return;
	}

	/**
	 * Loads the values of an account profile into internal variables.
	 *
	 * @param array $profile hash array with profile values (identifier => value)
	 */
	function load_profile($profile) {
		// profile mappings in meta data
		parent::load_profile($profile);
		// add extension
		if (isset($profile['hostObject_addExt'][0]) && ($profile['hostObject_addExt'][0] == "true")) {
			if (!in_array('hostObject', $this->attributes['objectClass'])) {
				$this->attributes['objectClass'][] = 'hostObject';
			}
		}
		$hostList = preg_split('/,[ ]*/', $profile['hostObject_host'][0]);
		$this->attributes['host'] = $hostList;

	}

	/**
	 * @inheritDoc
	 */
	public function getListAttributeDescriptions(ConfiguredType $type): array {
		return [
			'host' => _('Host list')
		];
	}

}
