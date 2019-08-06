<?php
namespace LAM\TOOLS\TESTS;
use \LAM\REMOTE\Remote;
use \htmlTitle;
use \htmlOutputText;
use \htmlResponsiveSelect;
use \htmlResponsiveInputCheckbox;
use \htmlButton;
use \htmlStatusMessage;
use \htmlImage;
use \htmlSubTitle;
use \Exception;
use \htmlResponsiveRow;

/*

  This code is part of LDAP Account Manager (http://www.ldap-account-manager.org/)
  Copyright (C) 2006 - 2018  Roland Gruber

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
* Tests the remote script.
*
* @author Roland Gruber
* @author Thomas Manninger
* @package tools
*/

/** security functions */
include_once(__DIR__ . "/../../lib/security.inc");
/** access to configuration options */
include_once(__DIR__ . "/../../lib/config.inc");

// start session
startSecureSession();
enforceUserIsLoggedIn();

// die if no write access
if (!checkIfWriteAccessIsAllowed()) die();

checkIfToolIsActive('toolTests');

setlanguage();

include '../../lib/adminHeader.inc';
echo "<div class=\"user-bright smallPaddingContent\">\n";
echo "<form action=\"lamdaemonTest.php\" method=\"post\">\n";

$container = new htmlResponsiveRow();
$container->add(new htmlTitle(_("Lamdaemon test")), 12);

$servers = explode(";", $_SESSION['config']->get_scriptServers());
$serverIDs = array();
$serverTitles = array();
for ($i = 0; $i < sizeof($servers); $i++) {
	$serverParts = explode(":", $servers[$i]);
	$serverName = $serverParts[0];
	$title = $serverName;
	if (isset($serverParts[1])) {
		$title = $serverParts[1] . " (" . $serverName . ")";
	}
	$serverIDs[] = $serverName;
	$serverTitles[$serverName] = $title;
}

if (isset($_POST['runTest'])) {
	lamRunTestSuite($_POST['server'], $serverTitles[$_POST['server']] , isset($_POST['checkQuotas']), $container);
}
else if ((sizeof($servers) > 0) && isset($servers[0]) && ($servers[0] != '')) {
	$serverOptions = array();
	for ($i = 0; $i < sizeof($servers); $i++) {
		$servers[$i] = explode(":", $servers[$i]);
		$serverName = $servers[$i][0];
		$title = $serverName;
		if (isset($servers[$i][1])) {
			$title = $servers[$i][1] . " (" . $serverName . ")";
		}
		$serverOptions[$title] = $serverName;
	}
	$serverSelect = new htmlResponsiveSelect('server', $serverOptions, array(), _("Server"));
	$serverSelect->setHasDescriptiveElements(true);
	$container->add($serverSelect, 12);

	$container->add(new htmlResponsiveInputCheckbox('checkQuotas', false, _("Check quotas")), 12);

	$container->addVerticalSpacer('1rem');

	$okButton = new htmlButton('runTest', _("Ok"));
	$okButton->colspan = 2;
	$container->addLabel($okButton);
	$container->addField(new htmlOutputText('&nbsp;', false));
}
else {
	$container->add(new htmlStatusMessage("ERROR", _('No lamdaemon server set, please update your LAM configuration settings.')), 12);
}

$tabindex = 1;
parseHtml(null, $container, array(), false, $tabindex, 'user');

echo "</form>\n";
echo "</div>\n";
include '../../lib/adminFooter.inc';


/**
 * Runs a test case of lamdaemon.
 *
 * @param string $command test command
 * @param boolean $stopTest specifies if test should be run
 * @param Remote $remote SSH connection
 * @param string $testText describing text
 * @param htmlResponsiveRow $container container for HTML output
 * @return boolean true, if errors occured
 */
function testRemoteCommand($command, $stopTest, $remote, $testText, $container) {
	$okImage = "../../graphics/pass.png";
	$failImage = "../../graphics/fail.png";
	// run remote command
	if (!$stopTest) {
		$container->add(new htmlOutputText($testText), 10, 4);
		flush();
		$lamdaemonOk = false;
		$output = $remote->execute($command);
		if ((stripos(strtolower($output), "error") === false) && ((strpos($output, 'INFO,') === 0) || (strpos($output, 'QUOTA_ENTRY') === 0))) {
			$lamdaemonOk = true;
		}
		if ($lamdaemonOk) {
			$container->add(new htmlImage($okImage), 2);
			$container->add(new htmlOutputText(_("Lamdaemon successfully run.")), 12, 6);
		}
		else {
			$container->add(new htmlImage($failImage), 2);
			if (!(strpos($output, 'ERROR,') === 0) && !(strpos($output, 'WARN,') === 0)) {
				// error messages from console (e.g. sudo)
				$container->add(new htmlStatusMessage('ERROR', $output), 12, 6);
			}
			else {
				// error messages from lamdaemon
				$parts = explode(",", $output);
				if (sizeof($parts) == 2) {
					$container->add(new htmlStatusMessage($parts[0], $parts[1]), 12, 6);
				}
				elseif (sizeof($parts) == 3) {
					$container->add(new htmlStatusMessage($parts[0], $parts[1], $parts[2]), 12, 6);
				}
				else {
					$container->add(new htmlOutputText($output), 12, 6);
				}
			}
			$stopTest = true;
		}
	}
	$container->addVerticalSpacer('0.5rem');
	return $stopTest;
}

/**
 * Runs all tests for a given server.
 *
 * @param String $serverName server ID
 * @param String $serverTitle server name
 * @param boolean $testQuota true, if Quotas should be checked
 * @param htmlResponsiveRow $container container for HTML output
 */
function lamRunTestSuite($serverName, $serverTitle, $testQuota, $container) {
	$SPLIT_DELIMITER = "###x##y##x###";
	$LAMDAEMON_PROTOCOL_VERSION = '5';
	$okImage = "../../graphics/pass.png";
	$failImage = "../../graphics/fail.png";

	$stopTest = false;

	$container->add(new htmlSubTitle($serverTitle), 12);

	// check script server and path
	$container->add(new htmlOutputText(_("Lamdaemon server and path")), 10, 4);
	if (!isset($serverName) || (strlen($serverName) < 3)) {
		$container->add(new htmlImage($failImage), 2);
		$container->add(new htmlOutputText(_("No lamdaemon server set, please update your LAM configuration settings.")), 12, 6);
	}
	elseif (($_SESSION['config']->get_scriptPath() == null) || (strlen($_SESSION['config']->get_scriptPath()) < 10)) {
		$container->add(new htmlImage($failImage), 2);
		$container->add(new htmlOutputText(_("No lamdaemon path set, please update your LAM configuration settings.")), 12, 6);
		$stopTest = true;
	}
	elseif (substr($_SESSION['config']->get_scriptPath(), -3) != '.pl') {
		$container->add(new htmlImage($failImage), 2);
		$container->add(new htmlOutputText(_("Lamdaemon path does not end with \".pl\". Did you enter the full path to the script?")), 12, 6);
		$stopTest = true;
	}
	else {
		$container->add(new htmlImage($okImage), 2);
		$container->add(new htmlOutputText(sprintf(_("Using %s as lamdaemon remote server."), $serverName)), 12, 6);
	}
	$container->addVerticalSpacer('0.5rem');

	// check Unix account of LAM admin
	$ldapUser = $_SESSION['ldap']->getUserName();
	if (!$stopTest) {
		$scriptUserName = $_SESSION['config']->getScriptUserName();
		if (empty($scriptUserName)) {
			$container->add(new htmlOutputText(_("Unix account")), 10, 4);
			$unixOk = false;
			$sr = @ldap_read($_SESSION['ldap']->server(), $ldapUser, "objectClass=posixAccount", array('uid'), 0, 0, 0, LDAP_DEREF_NEVER);
			if ($sr) {
				$entry = @ldap_get_entries($_SESSION['ldap']->server(), $sr);
				$userName = $entry[0]['uid'][0];
				if ($userName) {
					$unixOk = true;
				}
			}
			if ($unixOk) {
				$container->add(new htmlImage($okImage), 2);
				$container->add(new htmlOutputText(sprintf(_("Using %s to connect to remote server."), $userName)), 12, 6);
			}
			else {
				$container->add(new htmlImage($failImage), 2);
				$container->add(new htmlOutputText(sprintf(_("Your LAM admin user (%s) must be a valid Unix account to work with lamdaemon!"), $ldapUser)), 12, 6);
				$stopTest = true;
			}
			$container->addVerticalSpacer('0.5rem');
		}
		else {
			$userName = $_SESSION['config']->getScriptUserName();
		}
	}

	// check SSH login
	$remote = new Remote();
	if (!$stopTest) {
		$container->add(new htmlOutputText(_("SSH connection")), 10, 4);
		flush();
		$sshOk = false;
		try {
			$remote->connect($serverName);
			$container->add(new htmlImage($okImage), 2);
			$container->add(new htmlOutputText(_("SSH connection established.")), 12, 6);
		}
		catch (Exception $e) {
			$container->add(new htmlImage($failImage), 2);
			$container->add(new htmlOutputText($e->getMessage()), 12, 6);
			$stopTest = true;
		}
	}
	$container->addVerticalSpacer('0.5rem');

	if (!$stopTest) {
		$stopTest = testRemoteCommand("+" . $SPLIT_DELIMITER . "test" . $SPLIT_DELIMITER . "basic", $stopTest, $remote, _("Execute lamdaemon"), $container);
	}

	if (!$stopTest) {
		$stopTest = testRemoteCommand("+" . $SPLIT_DELIMITER . "test" . $SPLIT_DELIMITER . "version" . $SPLIT_DELIMITER . $LAMDAEMON_PROTOCOL_VERSION, $stopTest, $remote, _("Lamdaemon version"), $container);
	}

	if (!$stopTest) {
		$stopTest = testRemoteCommand("+" . $SPLIT_DELIMITER . "test" . $SPLIT_DELIMITER . "nss" . $SPLIT_DELIMITER . "$userName", $stopTest, $remote, _("Lamdaemon: check NSS LDAP"), $container);
		if (!$stopTest && $testQuota) {
			$stopTest = testRemoteCommand("+" . $SPLIT_DELIMITER . "test" . $SPLIT_DELIMITER . "quota", $stopTest, $remote, _("Lamdaemon: Quota module installed"), $container);
			$stopTest = testRemoteCommand("+" . $SPLIT_DELIMITER . "quota" . $SPLIT_DELIMITER . "get" . $SPLIT_DELIMITER . "user", $stopTest, $remote, _("Lamdaemon: read quotas"), $container);
		}
	}
	$remote->disconnect();

	$container->addVerticalSpacer('1rem');
	$endMessage = new htmlOutputText(_("Lamdaemon test finished."));
	$endMessage->colspan = 5;
	$container->add($endMessage, 12);
}

?>
