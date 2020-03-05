/**
 * This file is part of Hercules.
 * http://herc.ws - http://github.com/HerculesWS/Hercules
 *
 * Copyright (C) 2020 The Mana World
 * Copyright (C) 2014 - 2018 Hercules Dev Team
 *
 * Hercules is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "common/hercules.h"
#include "char/char.h" // chr
#include "char/inter.h" // sql_handle
#include "common/memmgr.h" // aStrdup, aFree
#include "common/mmo.h" // NAME_LENGTH
#include "common/conf.h" // libconfig
#include "common/strlib.h" // safestrncpy
#include "common/sql.h" // SQL, SQL_ERROR

#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"reserved",          // Plugin name
	SERVER_TYPE_CHAR,    // Which server types this plugin works with?
	"0.1.0",             // Plugin version
	HPM_VERSION,         // HPM Version (don't change, macro is automatically updated)
};

static bool enable_name_reservation = true;
static char *char_reservation_db = NULL;
#define DEFAULT_TABLE_NAME "char_reservation"


/**
 * Checks if the given name is valid for a new character.
 *
 * @param name The name to check.
 * @param esc_name Escaped version of the name, optional for faster processing.
 * @retval 0 if the name is valid.
 * @retval -1 if the name already exists or is reserved
 * @retval -2 if the name is too short or contains special characters.
 * @retval -5 if the name contains forbidden characters.
 */
static int check_char_name_post (int retVal, const char *name, const char *esc_name)
{
	if (retVal != 0) {
		return retVal; // already failed so no check needed
	}

	if (!enable_name_reservation || char_reservation_db == NULL) {
		return retVal; // lookup is disabled
	}

	char esc_name2[NAME_LENGTH * 2 + 1];
	if (esc_name == NULL) {
		SQL->EscapeStringLen(inter->sql_handle, esc_name2, name, strnlen(name, NAME_LENGTH));
		esc_name = esc_name2;
	}

	// this is case-insensitive (disallow nAme)
	// TODO: check for name_ignoring_case (should be exposed in chr->)

	if (SQL_ERROR == SQL->Query(inter->sql_handle, "SELECT 1 FROM `%s` WHERE `name` = '%s' LIMIT 1", char_reservation_db, esc_name)) {
		Sql_ShowDebug(inter->sql_handle);
		return -1;
	}

	if (SQL->NumRows(inter->sql_handle) > 0) {
		return -1; // name is reserved
	} else {
		return 0;
	}
}


static void config_check_sql_reservation (const char *key __attribute__ ((unused)), const char *val)
{
	enable_name_reservation = config_switch(val) ? true : false;
}

static void config_char_reservation_db (const char *key __attribute__ ((unused)), const char *val)
{
	if (char_reservation_db != NULL)
		aFree(char_reservation_db);

	char_reservation_db = aStrdup(val);
}

static void defaults (void)
{
	if (char_reservation_db == NULL) {
		CREATE(char_reservation_db, char, 17);
		safestrncpy(char_reservation_db, DEFAULT_TABLE_NAME, 17);
	}
}

HPExport void server_preinit (void)
{
	if (SERVER_TYPE == SERVER_TYPE_CHAR) {
		defaults();

		addCharConf("char_configuration/player/name/check_sql_reservation", config_check_sql_reservation);
		addCharInterConf("inter_configuration/database_names/char_reservation_db", config_char_reservation_db);
	}
}

HPExport void plugin_init (void)
{
	if (SERVER_TYPE == SERVER_TYPE_CHAR) {
		addHookPost(chr, check_char_name, check_char_name_post);
	}
}

HPExport void plugin_final (void)
{
	if (SERVER_TYPE == SERVER_TYPE_CHAR) {

		if (char_reservation_db != NULL) {
			aFree(char_reservation_db);
		}
	}
}
