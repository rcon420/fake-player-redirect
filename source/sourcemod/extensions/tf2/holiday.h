/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod Team Fortress 2 Extension
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#ifndef _INCLUDE_SOURCEMOD_HOLIDAY_H_
#define _INCLUDE_SOURCEMOD_HOLIDAY_H_

#include "extension.h"
#include <ISDKTools.h>

class HolidayManager : public IPluginsListener
{
public:
	HolidayManager();

public:
	void OnSDKLoad(bool bLate);
	void OnSDKUnload();
	void OnServerActivated();

public: //IPluginsListener
	void OnPluginLoaded(IPlugin *plugin);
	void OnPluginUnloaded(IPlugin *plugin);

public:
	bool Hook_IsHolidayActive(int holiday);
	void Hook_LevelShutdown();

private:
	bool IsHookEnabled() const { return m_iHookID != 0; }
	void *GetGameRules();
	void HookIfNecessary();
	void UnhookIfNecessary();

private:
	int m_iHookID;
	IForward *m_isHolidayForward;
	bool m_bInMap;
};

inline void *HolidayManager::GetGameRules()
{
	if (!g_pSDKTools)
		return NULL;

	return g_pSDKTools->GetGameRules();
}

extern HolidayManager g_HolidayManager;

#endif //_INCLUDE_SOURCEMOD_HOLIDAY_H_
