<?php
/*
$Id$

  This code is part of LDAP Account Manager (http://www.sourceforge.net/projects/lam)
  Copyright (C) 2003  Roland Gruber

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

echo ("<?xml version=\"1.0\" encoding=\"ISO-8859-15\"?>\n");
echo ("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n");

?>

<html>

<head>
	<title></title>
	<link rel="stylesheet" type="text/css" href="../style/layout.css">
</head>

<body>
<table border=0 width="100%">
	<tr>
    	<td width="100" align="left"><a href="./profedit/profilemain.php" target="mainpart"><?php echo _("Profile Editor") ?></a></td>
		<td rowspan=3 colspan=3 align="center">
			<a href="http://lam.sf.net" target="new_window"><img src="../graphics/banner.jpg" border=1 alt="LDAP Account Manager"></a>
		</td>
	<td width="100" align="right" height=20><a href="./logout.php" target="_top"><big><b><?php echo _("Logout") ?></b></big></a></td>
	</tr>
	<tr>
    	<td align="left"><a href="ou_edit.php" target="mainpart"><?php echo _("OU Editor") ?></a></td>
		<td rowspan=2></td>
	</tr>
	<tr>
    	<td align="left"><a href="masscreate.php" target="mainpart"><?php echo _("File Upload") ?></a></td>
	</tr>
	<tr><td colspan=5>&nbsp;</td></tr>
	<tr>
		<td></td>
		<td width="200" align="center"><a href="./lists/listusers.php" target="mainpart"> <?php echo _("Users");?> </a></td>
		<td width="200" align="center"><a href="./lists/listgroups.php" target="mainpart"> <?php echo _("Groups");?> </a></td>
		<td width="200" align="center"><a href="./lists/listhosts.php" target="mainpart"> <?php echo _("Hosts");?> </a></td>
		<td></td>
	</tr>
</table>
</body>
</html>
