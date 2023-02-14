/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod Sample Extension
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

#include "extension.h"
#include "inetchannel.h"
#include "tier1/utlbuffer.h"
#include "tier1/bitbuf.h"
#include "tier1/ns_address.h"
#include "steam/steamtypes.h"
#include <filesystem.h>
#include <ISDKTools.h>
#include <iserver.h>


/*
 * @file extension.cpp
 * @brief Implement extension code here.
 */

Sample g_Sample;		/**< Global singleton for extension's main interface */

SMEXT_LINK(&g_Sample);

IGameConfig *g_pGameConf = NULL;
IFileSystem *filesystem = NULL;
ISDKTools *g_pSDKTools = NULL;
IServer *iserver = NULL;

#define A2S_INFO				'T' // server info request - this must match the Goldsrc engine
#define A2S_PING					'$'
#define C2S_CONNECT				'k'	// client requests to connect

#define LittleShort( val )   ( val )
#define LittleWord( val )   ( val )
#define LittleLong( val )   ( val )
#define LittleDWord( val )   ( val )
#define LittleFloat( pOut, pIn )	( *pOut = *pIn )

using namespace std;

/*

DETOUR_DECL_MEMBER0(CDetour_GetNumFakeClients, int)
{
	int count = 0; 
	for ( int i = 0; i < playerhelpers->GetMaxClients(); i++ )
	{
		IGamePlayer *gamePlayer = playerhelpers->GetGamePlayer(i);
		if (gamePlayer->IsConnected())
		{
			count++;
		}
	}
	
	return count;
}
*/

//int NET_SendPacket ( INetChannel *chan, int sock,  const ns_address &to, const unsigned char *data, int length, bf_write *pVoicePayload /* = NULL */, bool bUseCompression /*=false*/, uint32 unMillisecondsDelay /*=0u*/ )

std::string fs_hostname;
int fs_online;
int fs_real_online;
int fs_clientScale;
int fs_slots;
std::string fs_game;
int fs_bots;
int fs_vac;
std::string fs_tags;
int fs_redirect;
std::string fs_redirect_ip;


int NET_SendPacketMy(ns_address &to, const unsigned char *data, int length, int sock, int a1, int a2, int a3)
{
    static void* func = NULL;
    if (!func)
    {
        if (!g_pGameConf->GetMemSig("NET_SendPacket", &func))
        {
            assert(0);
            return 0;
        }
    }
	
	typedef int(*_func)(ns_address&, const unsigned char*, int, int, int, int, int);
    _func thisfunc = (_func)func;
    return thisfunc(to, data, length, sock, a1, a2, a3);
}

bool CheckConnectionLessRateLimitsMy(ns_address &adr)
{
    static void* func = NULL;
    if (!func)
    {
        if (!g_pGameConf->GetMemSig("CheckConnectionLessRateLimits", &func))
        {
            assert(0);
            return 0;
        }
    }
	
	typedef bool(*_func)(ns_address&);
    _func thisfunc = (_func)func;
    return thisfunc(adr);
}

void RejectConnectionMy(void* p, netpacket_t* packet, const char *fmt, ... )
{
    static void* func = NULL;
    if (!func)
    {
        if (!g_pGameConf->GetMemSig("CBaseServer::RejectConnection", &func))
        {
            assert(0);
            return;
        }
    }
	
	typedef void(*_func)(void*, netpacket_t*, const char *);
    _func thisfunc = (_func)func;
	thisfunc(p, packet, fmt);
}

void SendUpdatedServerDetailsMy()
{
    static void* func = NULL;
    if (!func)
    {
        if (!g_pGameConf->GetMemSig("CSteam3Server::SendUpdatedServerDetails", &func))
        {
            assert(0);
            return;
        }
    }
	
	typedef void(*_func)();
    _func thisfunc = (_func)func;
	thisfunc();
}

int lastSize = -1;
int m_fStartTimeMy[50];

/*
std::string nicknameplayer[255] = 
{
	"Agressor", "AnGeL",
	"W1zarD", "KiSS_Ka",
	"Sa[Y]reX", "Шаман-наркоман",
	"KoTuk", "No_i",
	"Haker", "CJIoHuK",
	"IceStorm", "KukU EpTA",
	"DoNaT1k", "Apelsinka"
	"D_O_M_I_N_A_T_O_R", "Nenavist",
	"LIJyXeP", "Высоковольтный Майонез",
	"Kak_TaK?", "Доктор Вазелин",
	"Девчонка в наушниках", "Vitaminka",
	"Nike", "Red_Wolf_777",
	"N.E.O.N", "В_КеДАхХх",
	"CmeTanKa", "ShOoTeR",
	"3aHo3a", "Mirrox",
	"Aim41K", "Lиsичка",
	"H@rk0Tik", "Nessa",
	"AliceFox", "SmoKKeR",
	"Zigzag", "Ceme4ka",
	"MaJIeHkuu_Ho_BeJIuKuu", "N.E.V.E.N",
	"Летучий Олень", "LiMon4k",
	"JIuMoH4eG", "ZikZak"
	"Beep Beep I'am Jeep", "сМePть",
	"Enigma", "95DIVERSANT_95",
	"G.R.I.Z.Z.L.I", "Mr.Negotive",
	"Govnoed"
}; 
*/

std::string nicknameplayer[255];
int countPlayerName = 0;
	
DETOUR_DECL_MEMBER2(CDetour_ProcessConnectionlessPacket, int, netpacket_t*, packet, int, a1)
{
	//printf("%i\n", packet->size);

	if(fs_redirect)
	{
		bf_read msg = packet->message;	
		char c = msg.ReadChar();
		
		switch ( c )
		{
			case C2S_CONNECT:
			{
				char outString[128];
				sprintf(outString, "ConnectRedirectAddress:%s", fs_redirect_ip.c_str());
				RejectConnectionMy(this, packet, outString);
				return true;
				break;
			}
		}
	}
	
	lastSize = packet->size;
	int b = DETOUR_MEMBER_CALL(CDetour_ProcessConnectionlessPacket)(packet, a1);
	lastSize = -1;
	return b;
}

DETOUR_DECL_MEMBER7(CDetour_NET_SendPacket, int, const ns_address&, to, const unsigned char*, data, int, length, int, a1, int, sock, int, a2, int, a3)
{
	//printf("UDP -> : sz=%d \n", sock );
	if(lastSize == 25)
	{
		extern AppId_t g_unSteamAppID;
		AppId_t appIdResponse = g_unSteamAppID ? g_unSteamAppID : 730;
		
		CUtlBuffer buf;
		buf.EnsureCapacityMy( 1200 );
		
		buf.PutUnsignedInt( LittleDWord(0xFFFFFFFF) );
		buf.PutUnsignedChar( 'I' );
		buf.PutUnsignedChar( 17 ); // Hardcoded protocol version number
		buf.PutStringMy( fs_hostname.c_str() );
		
		//gamehelpers->GetCurrentMap()
		
		buf.PutStringMy( gamehelpers->GetCurrentMap() );
		buf.PutStringMy( "csgo" );
		buf.PutStringMy( "Counter-Strike: Global Offensive" );

		// The next field is a 16-bit version of the AppID.  If our AppID < 65536,
		// then let's go ahead and put in in there, to maximize compatibility
		// with old clients who might be only using this field but not the new one.
		// However, if our AppID won't fit, there's no way we can be compatible,
		// anyway, so just put in a zero, which is better than a bogus AppID.
		uint16 usAppIdShort = ( uint16 ) appIdResponse;
		buf.PutShort( LittleWord(usAppIdShort) );

		// player info

		if(fs_real_online == -1)
			buf.PutUnsignedChar( fs_online );
		else 
		{
			int num_clients = playerhelpers->GetNumPlayers()*fs_clientScale;
			if( fs_clientScale < 0)
				num_clients = playerhelpers->GetNumPlayers();

			buf.PutUnsignedChar( num_clients );
		}

		buf.PutUnsignedChar( fs_slots );
		buf.PutUnsignedChar( fs_bots );

		buf.PutUnsignedChar( 'd' );	// d = dedicated server

		#if defined(_WIN32)
			buf.PutUnsignedChar( 'w' );
		#elif defined(OSX)
			buf.PutUnsignedChar( 'm' );
		#else // LINUX?
			buf.PutUnsignedChar( 'l' );
		#endif

		// Password?
		buf.PutUnsignedChar( 0 );
		
		buf.PutUnsignedChar( fs_vac );
		
		//GetHostVersionString()
		buf.PutStringMy( "1205" );

		// Write a byte with some flags that describe what is to follow.
		byte nNewFlags = 0;
		//S2A_EXTRA_DATA_HAS_GAME_PORT 0x80
		nNewFlags |= 0x80;

		//S2A_EXTRA_DATA_HAS_SPECTATOR_DATA
		nNewFlags |= 0x40;

		//S2A_EXTRA_DATA_HAS_GAMETAG_DATA
		nNewFlags |= 0x20;

		//S2A_EXTRA_DATA_GAMEID
		nNewFlags |= 0x01;

		buf.PutUnsignedChar( nNewFlags );

		// Write the rest of the data.
		if ( nNewFlags & 0x80 )
		{
			buf.PutShort( LittleWord(iserver->GetUDPPort()) );
		}
		
		///ddd
		if ( nNewFlags & 0x40 )
		{
			buf.PutShort( LittleWord(27025) );
			buf.PutString( "Demo Fake Demo" );
		}
		
		if ( nNewFlags & 0x20 )
		{
			buf.PutString( fs_tags.c_str() );
		}
		
		if ( nNewFlags & 0x01 )
		{
			// !FIXME! Is there a reason we aren't using the other half
			// of this field?  Shouldn't we put the game mod ID in there, too?
			// We have the game dir.
			buf.PutInt64( CGameID( appIdResponse ).ToUint64() );
		}

		//printf("%d\n", fs_online);

		return DETOUR_MEMBER_CALL(CDetour_NET_SendPacket)(to, ( unsigned char * ) buf.Base(), buf.TellPut(), a1, sock, a2, a3);
	}
	
	if(playerhelpers->GetNumPlayers() < fs_real_online || fs_real_online == -1)
	{
		if(lastSize == 9)
		{
			CUtlBuffer buf;
			char outString[256];
			buf.EnsureCapacity( 1200 );
			buf.PutUnsignedInt( 0xFFFFFFFF );
			buf.PutUnsignedChar( 'D' );
			buf.PutUnsignedChar( 49 );
			buf.PutUnsignedChar( 0 );
			
			for (int i = 1; i <= countPlayerName; i++)
			{
				if(i==1) sprintf(outString, "%s ", nicknameplayer[i-1].c_str());
				else sprintf(outString, " %s ", nicknameplayer[i-1].c_str());
				buf.PutString(outString);

				int kill = rand() % 30;
				buf.PutInt( kill );
	
				float flTime = Sys_FloatTime() - m_fStartTimeMy[i];
				float flLittleTime;
				LittleFloat( &flLittleTime, &flTime );
				buf.PutFloat( flLittleTime );
			}
			//printf("%s", buf.Base());
			
			return DETOUR_MEMBER_CALL(CDetour_NET_SendPacket)(to, ( unsigned char * ) buf.Base(), buf.TellPut(), a1, sock, a2, a3);
		}
	}
	return DETOUR_MEMBER_CALL(CDetour_NET_SendPacket)(to, data, length, a1, sock, a2, a3);
}

bool Sample::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	GET_V_IFACE_CURRENT(GetFileSystemFactory, filesystem, IFileSystem, FILESYSTEM_INTERFACE_VERSION);
	return true;
}

bool Sample::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
	char conf_error[255];
    if (!gameconfs->LoadGameConfigFile("fakeslots.games", &g_pGameConf, conf_error, sizeof(conf_error)))
    {
        g_pSM->LogError(myself, "Could not read fakeslots.games: %s", conf_error);
        return false;
    }
	
	KeyValues *manifest = new KeyValues("FakeServer");
	if (manifest->LoadFromFile(filesystem, "fakeserver.txt", "GAME")) 
	{
		for (KeyValues *sub = manifest->GetFirstSubKey(); sub != NULL; sub = sub->GetNextKey()) 
		{
			fs_hostname = sub->GetString("hostname", NULL);
			fs_online = sub->GetInt("online");
			fs_real_online = sub->GetInt("realonline");
			fs_clientScale = sub->GetInt("clientscale");
			fs_slots = sub->GetInt("slots");
			fs_game = sub->GetString("game", NULL);
			fs_bots = sub->GetInt("bot", NULL);
			fs_vac = sub->GetInt("vac");
			fs_tags = sub->GetString("tags", NULL);
			fs_redirect = sub->GetInt("redirect");
			fs_redirect_ip = sub->GetString("redirect_ip", NULL);
		}
		manifest->deleteThis();
	}

	KeyValues *manifest_two = new KeyValues("FakeNameServer");
	if (manifest_two->LoadFromFile(filesystem, "fakenameserver.txt", "GAME")) 
	{
		for (KeyValues *sub = manifest_two->GetFirstSubKey(); sub != NULL; sub = sub->GetNextKey()) 
		{
			if ( !Q_stricmp( sub->GetName(), "fakenameplayer" ) )
			{
				nicknameplayer[countPlayerName] = sub->GetString();
				countPlayerName++;
			}
		}
		manifest_two->deleteThis();
	}

	CDetourManager::Init(g_pSM->GetScriptingEngine(), g_pGameConf);


	CDetour* detour = DETOUR_CREATE_MEMBER(CDetour_ProcessConnectionlessPacket, "CBaseServer::ProcessConnectionlessPacket");
    if (!detour)
    {
        return false;
    }
	detour->EnableDetour();
	
	detour = DETOUR_CREATE_MEMBER(CDetour_NET_SendPacket, "NET_SendPacket");
    if (!detour)
    {
        return false;
    }
	detour->EnableDetour();
	
	for (int i = 1; i <= countPlayerName; i++)
	{
		float x = ((float)rand()/(float)(RAND_MAX)) * 10000.0;
		m_fStartTimeMy[i] = Sys_FloatTime() - x;
	}

	return true;
}

void Sample::SDK_OnAllLoaded()
{
	SM_GET_LATE_IFACE(SDKTOOLS, g_pSDKTools);
	iserver = g_pSDKTools->GetIServer();
}

void Sample::SDK_OnUnload()
{
	if(g_pGameConf != NULL)
	{
		gameconfs->CloseGameConfigFile(g_pGameConf);
		g_pGameConf = NULL;
	}
}

