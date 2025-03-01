<?php
namespace LAM\HELP;
use \LAM\TYPES\TypeManager;
/*

  This code is part of LDAP Account Manager (http://www.ldap-account-manager.org/)
  Copyright (C) 2003 - 2006 Michael Duergner
                2003 - 2024 Roland Gruber

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


  LDAP Account Manager help topics.
*/

/**
 * LDAP Account Manager help entries.
 *
 * @author Michael Duergner
 * @author Roland Gruber
 * @package Help
 */

// generate help entry for translated list attributes
$entry206Example = "";
if (isset($_SESSION['conf_config'])) {
	$typeManager = new TypeManager($_SESSION['conf_config']);
	$types = $typeManager->getConfiguredTypes();
	foreach ($types as $type) {
		$entry206Example .= "<b>" . $type->getAlias() . ":</b><br>\n";
		$descriptions = $type->getBaseType()->getListAttributeDescriptions();
		$attributes = array_keys($descriptions);
		for ($a = 0; $a < sizeof($attributes); $a++) {
			$entry206Example .= "#" . $attributes[$a] . ": " . $descriptions[$attributes[$a]];
			if ($a < (sizeof($attributes) - 1)) {
				$entry206Example .= ", ";
			}
		}
		$entry206Example .= "<br><br>";
	}
}

$helpArray = [
				// 200 - 299
				// configuration wizard
				// configuration login
				// config profile management
				"200" => ["Headline" => _("Login"),
					"Text" => _("Please enter the configuration password. This is NOT your LDAP password. It is stored in your .conf-file. If this is the first time you log in, enter \"lam\".")
				],
				"201" => ["Headline" => _("Server address"),
					"Text" => _("This is the server address of your LDAP server. Use ldap:// for unencrypted LDAP connections or TLS encrypted connections. LDAP+SSL (LDAPS) encrypted connections are specified with ldaps://. The port value is optional.") .
						" " . _("TLS cannot be combined with ldaps://.") .
						"<br><br><b> " .
						_("Examples") .
						":</b><br><br> " .
						_("ldap://localhost:389 connects to localhost using a standard LDAP connection on port 389") .
						"<br> " .
						_("ldaps://ldap.domain.com connects to ldap.domain.com using an encrypted LDAP connection.") .
						"<br><br><br><b> " .
						_("Note") .
						":</b><br><br>" .
						_("When using ldaps:// or TLS be sure to use exactly the same IP/domain name as in your certificate!")
				],
				"202" => ["Headline" => _("LDAP suffix"),
					"Text" => _("This is the suffix of the LDAP tree from where to search for LDAP entries. Only entries in this subtree will be displayed in the account list. When creating a new account this will be the DN where it is saved.") .
						"<br><br><b>".
						_("Example").
						":</b><br><br>".
						_("ou=People,dc=yourcompany,dc=com will read and store all accounts in this subtree.")],
				"203" => ["Headline" => _("Tree suffix"),
					"Text" => _("This is the suffix for the LDAP tree viewer.") . ' ' . _("Multiple values are separated by semicolon.") .
						"<br><br><b>" .
						_("Example") .
						":</b><br><br>" .
						_("dc=yourcompany,dc=com") . '<br>' .
						_('dc=yourcompany,dc=com;cn=config')
				],
				"204" => ["Headline" => _("SSL certificate"),
					"Text" => _("This is only needed for TLS/SSL connections. By default, LAM will use the certificate authorities installed on your system. If you have a private CA in your company you can upload your CA certificates here and override the system certificates.")
				],
				"205" => ["Headline" => _("Follow referrals"),
					"Text" => _("Specifies if LAM should automatically follow referrals. Activate if you use referrals in your LDAP directory.")
				],
				"206" => ["Headline" => _("List attributes"),
					"Text" => _("This is the list of attributes to show in the account list. The entries can either be predefined values, \"#attribute\", or individual ones, \"attribute:description\". Several entries are separated by semicolons.") .
						"<br><br><br><big><b>" .
						_("Example") . ": </b></big>#homeDirectory;#uid;#uidNumber;#gidNumber;mail:Mail address<br><br>" .
						"<big><b>" . _("Predefined values") . ":</b></big><br><br>" . $entry206Example
				],
				"207" => ["Headline" => _("Valid users"),
					"Text" => _("This is a list of valid DN entries of all users that are allowed to login to LDAP Account Manager. Please enter one DN per line.") .
						"<br><br><b>" .
						_("Example") .
						": </b><br>cn=admin,dc=yourdomain,dc=org<br>cn=manager,dc=yourdomain,dc=org"
				],
				"208" => ["Headline" => _("Maximum list entries"),
					"Text" => _("This is the number of rows to show in the account list. If more entries are found the list will be split into several pages.")
				],
				"209" => ["Headline" => _("Default language"),
					"Text" => _("This defines the language of the login window and sets this language as the default language. Users can change the language at login.")
				],
				"210" => ["Headline" => _("Script path"),
					"Text" => _("This is the absolute path to an external script for setting quotas and creating home directories.")
				],
				"211" => ["Headline" => _("Enforce language"),
					"Text" => _("If enabled then the default language will be enforced and cannot be chosen by the user.")
				],
				"212" => ["Headline" => _("Change password"),
					"Text" => _("If you want to change the current preferences password, please enter it here.")
				],
				"213" => ["Headline" => _('Time zone'),
					"Text" => _('Please select your local time zone.')
				],
				"214" => ["Headline" => _("Cache timeout"),
					"Text" => _("This is the time in minutes which LAM caches its LDAP searches. Shorter times will stress LDAP more but decrease the possibility that changes are not identified.")
				],
				"215" => ["Headline" => _("Access level"),
					"Text" => _("You can specify if LAM allows full write access, password changes or only read access.")
				],
				"217" => ["Headline" => _("Account types and modules"),
					"Text" => _("Here you can select which plugins you want to use for account management.") . "<br><br>"
						. _("Account types define which sorts of LDAP entries (e.g. users and groups) should be managed. The account modules define which properties (e.g. Unix and Samba) can be edited.")
				],
				"218" => ["Headline" => _("Script servers"),
					"Text" => _("This is a list of the servers where the lamdaemon scripts are stored. LDAP Account Manager will make a SSH connection to the servers with the user name and password provided at login. Multiple servers are separated by semicolons. You can append a descriptive name after a colon.") . "<br>"
						. _("If your server runs on another port then add a comma and the port number after the server.") . "<br><br>"
						. _("Examples") . ": <br><b>my.server.name<br>127.0.0.1:LOCAL;192.168.0.2,12345:Servername<br>my.server.name:SERVER:/prefix</b>"
				],
				"219" => ["Headline" => _("Rights for the home directory"),
					"Text" => _("This defines the rights for the home directories which are created by lamdaemon.")
				],
				"220" => ["Headline" => _("Login method"),
					"Text" => _("The number of users who may login to LAM is restricted. This can be either a fixed list of DNs or LAM can search LDAP to find a DN which matches the given user name.")
				],
				"221" => ["Headline" => _("LDAP search"),
					"Text" => _("Please enter the LDAP suffix where LAM should start to search for users. The LDAP filter needs to match the given user name to exactly one DN. The value \"%USER%\" will be replaced by the user name from the login page.")
				],
				"222" => ["Headline" => _("LDAP search limit"),
					"Text" => _("Here you can set a limit for LDAP searches. This will restrict the number of results for LDAP searches. Please use this if LAM's LDAP queries produce too much load.")
				],
				"223" => ["Headline" => _("HTTP authentication"),
					"Text" => _("If enabled then LAM will use user and password that is provided by the web server via HTTP authentication.")
				],
				"224" => ["Headline" => _("Bind user and password"),
					"Text" => _("Here you can specify the DN and password of the bind user that will be used for the LDAP search. This is required if your LDAP server does not allow anonymous access.")
				],
				"225" => ["Headline" => _('Base URL'),
					"Text" => _("Please enter the base URL of your webserver (e.g. https://www.example.com). This is used to generate links in emails.")
				],
				"226" => ["Headline" => _("Authentication method"),
					"Text" => _("Please select how users authenticate at the login screen.")
				],
				"230" => ["Headline" => _("Profile management") . " - " . _("Add profile"),
					"Text" => _("Please enter the name of the new profile and the password to change its settings. Profile names may contain letters, numbers and -/_.")
				],
				"231" => ["Headline" => _("Profile management") . " - " . _("Rename profile"),
					"Text" => _("Please enter the new name of the profile. The name may contain letters, numbers and -/_.")
				],
				"232" => ["Headline" => _("Profile management") . " - " . _("Delete profile"),
					"Text" => _("This will delete the selected profile.")
				],
				"233" => ["Headline" => _("Profile management") . " - " . _("Set profile password"),
					"Text" => _("This changes the password of the selected profile.")
				],
				"234" => ["Headline" => _("Profile management") . " - " . _("Change default profile"),
					"Text" => _("This changes the profile which is selected by default at login.")
				],
				"235" => ["Headline" => _("Change master password"),
					"Text" => _("If you want to change your master configuration password, please enter it here.")
				],
				"236" => ["Headline" => _("Master password"),
					"Text" => _("Please enter the master configuration password. This is NOT your LDAP password. It is stored in your config.cfg file. If this is the first time you log in, enter \"lam\".")
				],
				"237" => ["Headline" => _("Base module"),
					"Text" => _("Every account type needs exactly one base module. This module provides a structural object class.")
				],
				"238" => ["Headline" => _("Session timeout"),
					"Text" => _("This is the time (in minutes) of inactivity after which a user is automatically logged off.")
				],
				"239" => ["Headline" => _("Log level"),
					"Text" => _("Please select your preferred log level. Messages with a lower level will not be logged.")
				],
				"240" => ["Headline" => _("Log destination"),
					"Text" => _("Here you can select where LAM should save its log messages. System logging will go to Syslog on Unix systems and event log on Windows. You can also select an extra file.")
				],
				"241" => ["Headline" => _("Allowed hosts"),
					"Text" => _("This is a list of IP addresses from hosts who may access LAM. You can use \"*\" as wildcard (e.g. 192.168.0.*).")
				],
				"242" => ["Headline" => _("Password policy"),
					"Text" => _("Here you can specify minimum requirements for passwords. The character classes are: lowercase, uppercase, numeric and symbols.")
				],
				"244" => ["Headline" => _('PHP error reporting'),
					"Text" => _('Defines if the PHP error reporting setting from php.ini is used or the setting preferred by LAM ("E_ALL & ~E_NOTICE"). If you do not develop LAM modules please use the default. This will prevent displaying messages that are useful only for developers.')
				],
				"246" => ["Headline" => _('Number of rules that must match'),
					"Text" => _('Specifies the number of above password rules that must be fulfilled.')
				],
				"247" => ["Headline" => _('Password must not contain user name'),
					"Text" => _('Specifies if the password must not contain the user name.')
				],
				"248" => ["Headline" => _('Password must not contain part of user/first/last name'),
					"Text" => _('Specifies if the password must not contain 3 or more characters of the user/first/last name.')
				],
				"249" => ["Headline" => _('External password check'),
					"Text" => _('Please specify the URL (e.g. "https://api.pwnedpasswords.com/range/{SHA1PREFIX}") of your external password check.')
				],
				"250" => ["Headline" => _("Filter"),
					"Text" => _("Here you can input simple filter expressions (e.g. 'value' or 'v*'). The filter is case-insensitive.")
				],
				"251" => ["Headline" => _("Remote server"),
					"Text" => _("Please enter the syslog remote server in format \"server:port\".")
				],
				"252" => ["Headline" => _("User DN"),
					"Text" => _("Please enter a part of the user's DN to search for registered devices.")
				],
				"253" => ["Headline" => _("Mail server"),
					"Text" => _("Please enter the server name and port of your SMTP server (e.g. localhost:25). If this setting is left empty then LAM will try to use a locally installed mail server.")
				],
				"254" => ["Headline" => _("User name"),
					"Text" => _("SMTP user name")
				],
				"255" => ["Headline" => _("Password"),
					"Text" => _("SMTP password")
				],
				"256" => ["Headline" => _("Encryption protocol"),
					"Text" => _("Please select if the connection should be encrypted via TLS, SSL or not at all.")
				],
				"257" => ["Headline" => _("Hide LDAP details on failed login"),
					"Text" => _("If activated, LAM will not display and details why the login to LAM failed. Use this if you have high security requirements and want to prevent e.g. user name guessing.")
				],
				"258" => ["Headline" => _("Mail attribute"),
					"Text" => _("This LDAP attribute contains the account's primary mail address.")
				],
				"259" => ["Headline" => _("Secondary mail attribute"),
					"Text" => _("This LDAP attribute contains the account's secondary mail address.")
				],
				"260" => ["Headline" => _("Additional LDAP filter"),
					"Text" => _('Use this to enter an additional LDAP filter (e.g. "(cn!=admin)") to reduce the number of visible elements for this account type.')
						. ' ' . _('You can use the wildcard @@LOGIN_DN@@ which will be substituted with the DN of the user who is currently logged in to LAM.')
						. ' ' . _('By default, LAM will show all accounts that match the selected account modules.')
				],
				"261" => ["Headline" => _("Hidden"),
					"Text" => _('Hidden account types will not show up in LAM. This is useful if you want to display e.g. only groups but still need to manage their members.')
				],
				"262" => ["Headline" => _("No new entries"),
					"Text" => _('If checked then the user will not be able to create new entries of this account type.')
				],
				"263" => ["Headline" => _("Disallow delete"),
					"Text" => _('If checked then the user will not be able to delete entries of this account type.')
				],
				"264" => ["Headline" => _('Custom label'),
					"Text" => _('Here you can overwrite the display name for this account type.')
				],
				"265" => ["Headline" => _('Read-only'),
					"Text" => _('Sets this account type to read-only.')
				],
				"266" => ["Headline" => _("Paged results"),
					"Text" => _("This is a workaround for Active Directory. Enable it if you get messages about size limit exceeded.")
				],
				"267" => ["Headline" => _('Template'),
					"Text" => _('Please select the template for the new server profile. You can either select an existing server profile or use one of the built-in templates.')
				],
				"268" => ["Headline" => _('Display name'),
					"Text" => _('This name is shown on the login page as server name. Defaults to server address if empty.')
				],
				"269" => ["Headline" => _('Referential integrity overlay'),
					"Text" => _('Activate this checkbox if you have any server side extension for referential integrity in place. LAM will then skip cleanup tasks like deletion of group memberships on account deletion.')
				],
				"270" => ["Headline" => _('Bind user and password'),
					"Text" => _('Please enter the DN and password to use for all jobs.')
				],
				"271" => ["Headline" => _('Database type'),
					"Text" => _('Please select the type of database to use for job data.')
				],
				"272" => ["Headline" => _('Cron configuration'),
					"Text" => _('Please add this line to your webserver\'s crontab.')
				],
				"273" => ["Headline" => _('Database host'),
					"Text" => _('Enter the host name of your database server.')
				],
				"274" => ["Headline" => _('Database port'),
					"Text" => _('Enter the port number of your database server. The default port will be used if empty.')
				],
				"275" => ["Headline" => _('Database user and password'),
					"Text" => _('Please enter the user name and password to connect to the database.')
				],
				"276" => ["Headline" => _('Database name'),
					"Text" => _('This is the database name on the server.')
				],
				'280' => ["Headline" => _('Allow setting specific passwords'),
					"Text" => _('Allows to set a specific password via input field.')
				],
				'281' => ["Headline" => _('Allow to display password on screen'),
					"Text" => _('Allows to display a randomly generated password on screen.')
				],
				'282' => ["Headline" => _('Default password output'),
					"Text" => _('Default method to output a random password.')
				],
				'283' => ["Headline" => _('Force password change by default'),
					"Text" => _('Enforce password change on next login by default.')
				],
				'284' => ["Headline" => _('User name'),
					"Text" => _('User name for SSH connection to lamdaemon server. If empty the user name of the person who is logged into LAM will be used.')
				],
				'285' => ["Headline" => _('SSH key file'),
					"Text" => _('Path to SSH key file to connect to lamdaemon server.') . ' ' . _('If empty then password authentication with the person\'s password who is logged into LAM will be used.')
				],
				'285a' => ["Headline" => _('SSH key file'),
					"Text" => _('Path to SSH key file to connect to lamdaemon server.')
				],
				'286' => ["Headline" => _('SSH key password'),
					"Text" => _('Password to unlock SSH key file.')
				],
				'287' => ["Headline" => _('Licence'),
					"Text" => _('Please enter your licence key.')
				],
				'288' => ["Headline" => _('Expiration warning'),
					"Text" => _('Please select how to be warned before your licence expires.')
				],
				'289' => ["Headline" => _('From address'),
					"Text" => _('This email address will be set as sender address of the mails.')
				],
				'290' => ["Headline" => _('TO address'),
					"Text" => _('This email address will be set as TO address for the mails.') . ' '
						. _("Multiple values are separated by semicolon.")
				],
				"291" => ["Headline" => _('Hide password prompt for expired password'),
					"Text" => _('Hides the password prompt when a user with expired password logs into LAM.')
				],
				"292" => ["Headline" => _('DN part to hide'),
					"Text" => _('Hides the given part of the DN when displaying a DN. E.g. if you set this to "dc=example,dc=com" then "ou=department,dc=example,dc=com" will be displayed as "ou=department". Use this if you have very long DNs.')
				],
				"293" => ["Headline" => _('Database type'),
					"Text" => _('Please select the type of database to use for all configuration data. Please install PHP MySQL PDO extension for MySQL support.')
				],
				"294" => ["Headline" => _('Cron command'),
					"Text" => _('Run this for global cleanup tasks. See manual for details.')
				],
				// 300 - 399
				// profile/PDF editor, file upload
				"301" => ["Headline" => _("RDN identifier"),
					"Text" => _("This is the identifier for the relative DN value. It must be one of the given allowed LDAP attributes (e.g. user accounts usually use \"uid\" while groups use \"cn\").")
				],
				"302" => ["Headline" => _("Overwrite"),
					"Text" => _("Set to true to overwrite an existing LDAP entry.")
				],
				"360" => ["Headline" => _("Profile name"),
					"Text" => _("Name under which the profile will be saved. If a profile with the same name exists, it will be overwritten.")
				],
				"361" => ["Headline" => _("DN suffix"),
					"Text" => _("The account will be saved under this LDAP suffix.")],
				"362" => ["Headline" => _("Import profiles"),
					"Text" => _("Here you can import account profiles from other server profiles (overwrite existing).")],
				"363" => ["Headline" => _("Export profile"),
					"Text" => _("Here you can export account profiles to other server profiles (overwrite existing). You may also export a profile to the global templates. In this case it will always be copied to all server profiles that do not yet have a profile with this name.")
				],
				"364" => ["Headline" => _("Global templates"),
					"Text" => _("Global templates will always be copied to all server profiles that do not yet have a profile with this name.")
				],
				"365" => ["Headline" => _("Global template logos"),
					"Text" => _("Global template logos will always be copied to all server profiles that do not yet have a logo with this name.")
				],
				// 400 - 499
				// account pages
				"400" => ["Headline" => _("DN suffix") . '/' . _("RDN identifier"),
					"Text" => _("The account will be saved under this LDAP suffix.") . '<br><br>' . _("This is the identifier for the relative DN value. It must be one of the given allowed LDAP attributes (e.g. user accounts usually use \"uid\" while groups use \"cn\").")
				],
				"401" => ["Headline" => _("Load profile"),
					"Text" => _("Here you can load an account profile to set default settings for your account. The \"default\" profile is automatically loaded for new accounts.")
				],
				"403" => ["Headline" => _("Create PDF file"),
					"Text" => _("Here you can select a PDF structure and export the account to a PDF file.")
				],
				"404" => ["Headline" => _("Password"),
					"Text" => _("Please enter the password which you want to set for this account. You may also generate a random password (12 characters) which will be displayed on your screen.")
				],
				"405" => ["Headline" => _("PDF structure"),
					"Text" => _("The PDF structure defines what information is exported as PDF file and how the pages are structured. You can manage the PDF structures in the PDF editor (under \"Tools\").")
				],
				"406" => ["Headline" => _("Force password change"),
					"Text" => _("If you set this option then the user has to change his password at the next login.")
				],
				"407" => ["Headline" => _("Send via mail"),
					"Text" => _("Sends the password to the user via mail. Please edit your LAM server profile to setup the mail settings.")
				],
				"408" => ["Headline" => _("Import PDF structures"),
					"Text" => _("Here you can import PDF structures from other server profiles (overwrite existing).")
				],
				"409" => ["Headline" => _("Export PDF structure"),
					"Text" => _("Here you can export PDF structures to other server profiles (overwrite existing). You may also export a structure to the global templates. In this case it will always be copied to all server profiles that do not yet have a structure with this name.")
				],
				"410" => ["Headline" => _("Alternate recipient"),
					"Text" => _("Here you can enter an alternative mail address for the password. To use the user's primary email address please leave the field blank.")
				],
				"411" => ["Headline" => _("Font"),
					"Text" => _("Please select the font for the PDF file. Dejavu will work on all systems but does not support e.g. Chinese and Japanese. The other fonts require that an appropriate font is installed on the system where the PDF is opened.")
				],
				// 500 - 599
				// LAM Pro
				"501" => ["Headline" => _("LDAP suffix"),
					"Text" => _("LAM will search for accounts in this part of the LDAP tree.")
				],
				"502" => ["Headline" => _("LDAP user and password"),
					"Text" => _("LAM will use this LDAP DN and password to search for accounts. It is sufficient to specify an account with read rights. If nothing is inserted then LAM will try to connect anonymously.")
				],
				"503" => ["Headline" => _("LDAP search attribute"),
					"Text" => _("This is needed to find the LDAP DNs of your user accounts. E.g. if you use \"uid\" and your user inputs \"miller\" then LAM will search for an account with uid=miller.")
				],
				"504" => ["Headline" => _("Login attribute label"),
					"Text" => _("This text should shortly describe your selected LDAP search attribute (e.g. email or user name).")
				],
				"505" => ["Headline" => _("Login caption"),
					"Text" => _("This text is displayed on top of the self service login page. You can also input HTML code here.")
				],
				"506" => ["Headline" => _("Main page caption"),
					"Text" => _("This text is displayed on top of the self service main page. You can also input HTML code here.")
				],
				"507" => ["Headline" => _("Add new group"),
					"Text" => _("Adds a new group element to the list of self service options. Use this to structure the input fields.")
				],
				"508" => ["Headline" => _("Add input fields"),
					"Text" => _("Adds a new self service option to the selected group element.")
				],
				"509" => ["Headline" => _("Page header"),
					"Text" => _("This HTML code will be placed on top of all self service pages. E.g. you can use this to place your custom logo. Any HTML code is permitted.")
				],
				"510" => ["Headline" => _("Additional CSS links"),
					"Text" => _("Here you can specify additional CSS links to change the layout of the self service pages. This is useful to adapt them to your corporate design. Please enter one link per line.")
				],
				"511" => ["Headline" => _("Password field label"),
					"Text" => _('This text is placed as label for the password field on the login page. LAM will use "Password" if you do not enter any text.')
				],
				"512" => ["Headline" => _("Additional LDAP filter"),
					"Text" => _('Use this to enter an additional LDAP filter (e.g. "(objectClass=passwordSelfReset)") to reduce the number of accounts who may use self service.')
				],
				"513" => ["Headline" => _('Use for all operations'),
					"Text" => _('By default, all modifications are done as the user that authenticated in self service. If active then LAM will use the connection user for all LDAP modifications and searches.')
				],
				"514" => ["Headline" => _('2-factor authentication'),
					"Text" => _('You can enable 2-factor authentication here (e.g. via mobile device).')
				],
				"515" => ["Headline" => _('Base URL'),
					"Text" => _('URL of external 2-factor authentication service.')
				],
				"515a" => ["Headline" => _('Base URLs'),
					"Text" => _('URLs of external 2-factor authentication service. Enter one per line.')
				],
				"516" => ["Headline" => _('Disable certificate check'),
					"Text" => _('This will disable the check of the SSL certificates for the 2-factor authentication service. Not recommended for production usage.')
				],
				"517" => ["Headline" => _('Label'),
					"Text" => _('Use this to overwrite the default label for the 2-factor input field. Default is "PIN+Token".')
				],
				"518" => ["Headline" => _('Caption'),
					"Text" => _('This text is displayed on top of the 2-factor page. You can also input HTML code here.')
				],
				"519" => ["Headline" => _('Optional'),
					"Text" => _('If checked then also users who did not setup a second factor are able to login.')
				],
				"520" => ["Headline" => _("Generate random password"),
					"Text" => _("This will set a random password and display it on the screen or send it to the user via mail. Please edit your LAM server profile to setup the mail settings.")
				],
				"521" => ["Headline" => _('Captcha'),
					"Text" => _('Please enter the site and secret/API key you got from your captcha provider.')
				],
				"522" => ["Headline" => _('Secure login'),
					"Text" => _('Protect the self service login with a captcha.')
				],
				"523" => ["Headline" => _('Base color'),
					"Text" => _('Background color for self service pages.')
				],
				"524" => ["Headline" => _('Client id'),
					"Text" => _('Please enter your client id for the verification API.')
				],
				"525" => ["Headline" => _('Secret key'),
					"Text" => _('Please enter your secret key for the verification API.')
				],
				"526" => ["Headline" => _('Login footer'),
					"Text" => _('This text is displayed as footer on the self service login page.')
				],
				"527" => ["Headline" => _('Main page footer'),
					"Text" => _('This text is displayed as footer on the self service main page.')
				],
				"528" => ["Headline" => _('User name attribute'),
					"Text" => _('The attribute (e.g. "uid") that contains the user name for the 2-factor service.')
				],
				"529" => ["Headline" => _('Domain'),
					"Text" => _('Please enter the WebAuthn domain. This is the public domain of the webserver (e.g. "example.com"). Do not include protocol or port.')
				],
				"530" => ["Headline" => _('Allow remembering device'),
					"Text" => _('Allows the user to remember the current device for a specified period of time. During this time the 2nd factor will not be checked anymore when logging in from this device.')
				],
				"531" => ["Headline" => _('Remember device period'),
					"Text" => _('Please specify how long a device should be remembered (e.g. "8h" or "1d").')
				],
				"532" => ["Headline" => _('Password to remember devices'),
					"Text" => _('This password is used to encrypt the data about remembering the current device. When the password is changed all users need to use the 2nd factor on the next login.')
				],
				"550" => ["Headline" => _("From address"),
					"Text" => _("This email address will be set as sender address of all password mails. If empty the system default (php.ini) will be used.")
				],
				"551" => ["Headline" => _("Subject"),
					"Text" => _('The subject of all password mails.')
				],
				"552" => ["Headline" => _("Text"),
					"Text" => _('The mail text of all password mails.') .
						'<br><br><b>' . _('Format') . ': </b><ul>' .
						'<li>' . _('You can use wildcards for LDAP attributes in the form @@attribute@@ (e.g. @@uid@@ for the user name).') . '</li>' .
						'<li>' . _('The wildcard for the new password is @@newPassword@@.') . '</li>'
				],
				"553" => ["Headline" => _("HTML format"),
					"Text" => _('Specifies if the mail should be sent as text or HTML.')
				],
				"554" => ["Headline" => _("Reply-to address"),
					"Text" => _("This email address will be set as reply-to address of all password mails.")
				],
				"555" => ["Headline" => _("Allow alternate address"),
					"Text" => _('Specifies if password mails may be sent to mail addresses other than the user\'s LDAP mail address.')
				],
				'556' => [
					"Headline" => _('To address'),
					"Text" => _('This email address will be set as To address of all mails.')
				],
				'557' => [
					"Headline" => _('To address'),
					"Text" => _('This email address will be set as To address of all mails.') . ' '
						. _('Multiple values are separated by comma.')
				],
				"560" => ["Headline" => _("Remember device"),
					"Text" => _('This will remember your current device. You will not need to provide your 2nd factor for a configured period of time.')
				],
				// 600 - 699
				// OU-editor, domain page
				"601" => ["Headline" => _("OU-Editor") . " - " . _("New organisational unit"),
					"Text" => _("This will create a new organisational unit under the selected one.")
				],
				"602" => ["Headline" => _("OU-Editor") . " - " . _("Delete organisational unit"),
					"Text" => _("This will delete the selected organisational unit. The OU has to be empty.")
				],
				// 700 - 749
				// multi edit tool
				"700" => ["Headline" => _('LDAP suffix'),
					"Text" => _('Please select the suffix where changes should be done.')
				],
				"701" => ["Headline" => _('LDAP filter'),
					"Text" => _('Use this to enter an additional LDAP filter (e.g. "(cn!=admin)") to reduce the number of entries to modify.')
				],
				"702" => ["Headline" => _('Operations'),
					"Text" => _('Please specify which attributes should be changed. The modify operation will also add an value if the attribute does not yet exist. To delete all values of an attribute please leave the value field empty.')
				],
				// 750 - 799
				// import/export
				"750" => ["Headline" => _('LDIF data'),
					"Text" => _('The input data must be formatted in LDIF format.')
				],
				"751" => ["Headline" => _('Base DN'),
					"Text" => _('The export will read entries of this DN.')
				],
				"752" => ["Headline" => _('Search filter'),
					"Text" => _('Please enter an LDAP filter to specify the exported entries.')
				],
				"753" => ["Headline" => _('Attributes'),
					"Text" => _('Please enter a comma separated list of attributes to export. Using "*" will export all attributes.')
				],
				"754" => ["Headline" => _('Include system attributes'),
					"Text" => _('Activate this option to export internal attributes that are not visible by default.')
				],
				// 800 - 899
				// jobs
				'800' => [
					"Headline" => _('From address'),
					"Text" => _('This email address will be set as sender address of the mails.')
				],
				'801' => [
					"Headline" => _('Reply-to address'),
					"Text" => _('This email address will be set as reply-to address of all mails.')
				],
				'802' => [
					"Headline" => _('Subject'),
					"Text" => _('The subject of all mails.')
				],
				'803' => [
					"Headline" => _('Text'),
					"Text" => _('The mail text of all mails.') .
							_('You can use wildcards for LDAP attributes in the form @@attribute@@ (e.g. @@uid@@ for the user name).')
							. ' ' . _('The expiration date can be added with @@EXPIRE_DATE_DDMMYYYY@@ or @@EXPIRE_DATE_YYYYMMDD@@.')
				],
				'804' => [
					"Headline" => _('Notification period'),
					"Text" => _('Please enter the number of days before password expiration to send out the email.')
				],
				'805' => [
					"Headline" => _('CC address'),
					"Text" => _('This email address will be set as CC address of all mails.')
				],
				'806' => [
					"Headline" => _('BCC address'),
					"Text" => _('This email address will be set as BCC address of all mails.')
				],
				'807' => [
					"Headline" => _('Delay'),
					"Text" => _('Delay this action by a number of days after account expiry.')
				],
				'808' => [
					"Headline" => _('Action'),
					"Text" => _('You can delete or move expired accounts.')
				],
				'809' => [
					"Headline" => _('Target DN'),
					"Text" => _('The expired accounts will be moved to this DN.')
				],
				'810' => [
					"Headline" => _('Text'),
					"Text" => _('The mail text of all mails.') .
						_('You can use wildcards for LDAP attributes in the form @@attribute@@ (e.g. @@uid@@ for the user name).')
						. ' ' . _('The managed groups need to be added with @@LAM_MANAGED_GROUPS@@.')
				],
				'811' => [
					"Headline" => _('Period'),
					"Text" => _('This defines how often the email is sent (e.g. each month).')
				],
				'812' => [
					"Headline" => _('Delay'),
					"Text" => _('Delay this action by a number of days after last successful login.')
				],
];

/* This is a sample help entry. Just copy this line an modify the values between the [] brackets.

	"[HelpNumber]" => ["Headline" => _("[Headline]"), "Text" => _("[Text]"), "SeeAlso" => ['link' => "[SeeAlso link]", 'text' => '[link text]']],
	"999" => ["Headline" => _("This is a sample entry"), "Text" => _("Sample text"), "SeeAlso" => ['link' => "http://www.ldap-account-manager.org/", 'text' => 'Project page']],



	Syntax:
	=======

		HelpNumber: numeric value
		Headline: topic of help entry
		Text: help text (ext = FALSE)
		SeeAlso: URL to additional information (ext = FALSE, optional)
		Link: relative URL to external help page (ext = TRUE)
*/
