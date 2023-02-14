#if defined( _WIN32 ) && !defined( _X360 )
#include <windows.h>
#endif

#include "tier0/dbg.h"
#include "netadr.h"
#include "tier1/strtools.h"
#include "steam/steamclientpublic.h"
#include "steam/steamtypes.h"

#if defined( _WIN32 ) && !defined( _X360 )
#define WIN32_LEAN_AND_MEAN
#include <winsock.h>
#elif !defined( _X360 )
#include <netinet/in.h> // ntohs()
#include <netdb.h>		// gethostbyname()
#include <sys/socket.h>	// getsockname()
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
#include "tier1/utlbuffer.h"

void netadr_t::ToString( char *pchBuffer, uint32 unBufferSize, bool onlyBase ) const
{

	if (type == NA_LOOPBACK)
	{
		V_strncpy( pchBuffer, "loopback", unBufferSize );
	}
	else if (type == NA_BROADCAST)
	{
		V_strncpy( pchBuffer, "broadcast", unBufferSize );
	}
	else if (type == NA_IP)
	{
		if ( onlyBase )
		{
			V_snprintf( pchBuffer, unBufferSize, "%i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
		}
		else
		{
			V_snprintf( pchBuffer, unBufferSize, "%i.%i.%i.%i:%i", ip[0], ip[1], ip[2], ip[3], ntohs(port));
		}
	}
	else
	{
		V_strncpy( pchBuffer, "unknown", unBufferSize );
	}
}

const char * CSteamID::Render() const
{
	// longest length of returned string is k_cBufLen
	//	[A:%u:%u:%u]
	//	 %u == 10 * 3 + 6 == 36, plus terminator == 37
	const int k_cBufLen = 37;

	const int k_cBufs = 4;	// # of static bufs to use (so people can compose output with multiple calls to Render() )
	static char rgchBuf[k_cBufs][k_cBufLen];
	static int nBuf = 0;
	char * pchBuf = rgchBuf[nBuf];	// get pointer to current static buf
	nBuf ++;	// use next buffer for next call to this method
	nBuf %= k_cBufs;

	if ( k_EAccountTypeAnonGameServer == m_steamid.m_comp.m_EAccountType )
	{
		V_snprintf( pchBuf, k_cBufLen, "[A:%u:%u:%u]", m_steamid.m_comp.m_EUniverse, m_steamid.m_comp.m_unAccountID, m_steamid.m_comp.m_unAccountInstance );
	}
	else if ( k_EAccountTypeGameServer == m_steamid.m_comp.m_EAccountType )
	{
		V_snprintf( pchBuf, k_cBufLen, "[G:%u:%u]", m_steamid.m_comp.m_EUniverse, m_steamid.m_comp.m_unAccountID );
	}
	else if ( k_EAccountTypeMultiseat == m_steamid.m_comp.m_EAccountType )
	{
		V_snprintf( pchBuf, k_cBufLen, "[M:%u:%u:%u]", m_steamid.m_comp.m_EUniverse, m_steamid.m_comp.m_unAccountID, m_steamid.m_comp.m_unAccountInstance );
	} 
	else if ( k_EAccountTypePending == m_steamid.m_comp.m_EAccountType )
	{
		V_snprintf( pchBuf, k_cBufLen, "[P:%u:%u]", m_steamid.m_comp.m_EUniverse, m_steamid.m_comp.m_unAccountID );
	} 
	else if ( k_EAccountTypeContentServer == m_steamid.m_comp.m_EAccountType )
	{
		V_snprintf( pchBuf, k_cBufLen, "[C:%u:%u]", m_steamid.m_comp.m_EUniverse, m_steamid.m_comp.m_unAccountID );
	}
	else if ( k_EAccountTypeClan == m_steamid.m_comp.m_EAccountType )
	{
		// 'g' for "group"
		V_snprintf( pchBuf, k_cBufLen, "[g:%u:%u]", m_steamid.m_comp.m_EUniverse, m_steamid.m_comp.m_unAccountID );
	}
	else if ( k_EAccountTypeChat == m_steamid.m_comp.m_EAccountType )
	{
		if ( m_steamid.m_comp.m_unAccountInstance & k_EChatInstanceFlagClan )
		{
			V_snprintf( pchBuf, k_cBufLen, "[c:%u:%u]", m_steamid.m_comp.m_EUniverse, m_steamid.m_comp.m_unAccountID );
		}
		else if ( m_steamid.m_comp.m_unAccountInstance & k_EChatInstanceFlagLobby )
		{
			V_snprintf( pchBuf, k_cBufLen, "[L:%u:%u]", m_steamid.m_comp.m_EUniverse, m_steamid.m_comp.m_unAccountID );
		}
		else // Anon chat
		{
			V_snprintf( pchBuf, k_cBufLen, "[T:%u:%u]", m_steamid.m_comp.m_EUniverse, m_steamid.m_comp.m_unAccountID );
		}
	}
	else if ( k_EAccountTypeInvalid == m_steamid.m_comp.m_EAccountType )
	{
		V_snprintf( pchBuf, k_cBufLen, "[I:%u:%u]", m_steamid.m_comp.m_EUniverse, m_steamid.m_comp.m_unAccountID );
	}
	else if ( k_EAccountTypeIndividual == m_steamid.m_comp.m_EAccountType )
	{
		if ( m_steamid.m_comp.m_unAccountInstance != k_unSteamUserDesktopInstance )
			V_snprintf( pchBuf, k_cBufLen, "[U:%u:%u:%u]", m_steamid.m_comp.m_EUniverse, m_steamid.m_comp.m_unAccountID, m_steamid.m_comp.m_unAccountInstance );
		else
			V_snprintf( pchBuf, k_cBufLen, "[U:%u:%u]", m_steamid.m_comp.m_EUniverse, m_steamid.m_comp.m_unAccountID );
	}
	else if ( k_EAccountTypeAnonUser == m_steamid.m_comp.m_EAccountType )
	{
		V_snprintf( pchBuf, k_cBufLen, "[a:%u:%u]", m_steamid.m_comp.m_EUniverse, m_steamid.m_comp.m_unAccountID );
	}
	else
	{
		V_snprintf( pchBuf, k_cBufLen, "[i:%u:%u]", m_steamid.m_comp.m_EUniverse, m_steamid.m_comp.m_unAccountID );
	}
	return pchBuf;
}
/*
netadrtype_t netadr_t::GetType() const
{
	return type;
}

unsigned short netadr_t::GetPort() const
{
	return BigShort( port );
}
*/
unsigned int netadr_t::GetIPNetworkByteOrder() const
{
	return *(unsigned int *)&ip;
}

unsigned int netadr_t::GetIPHostByteOrder() const
{
	return BigDWord( GetIPNetworkByteOrder() );
}

void CUtlBuffer::AddNullTermination( int nPut )
{
	if ( nPut > m_nMaxPut )
	{
		if ( !IsReadOnly() && ((m_Error & PUT_OVERFLOW) == 0)  )
		{
			// Add null termination value
			if ( CheckPut( 1 ) )
			{
				m_Memory[nPut - m_nOffset] = 0;
			}
			else
			{
				// Restore the overflow state, it was valid before...
				m_Error &= ~PUT_OVERFLOW;
			}
		}
		m_nMaxPut = nPut;
	}		
}

AppId_t g_unSteamAppID = k_uAppIdInvalid;


void CUtlBuffer::PutStringMy( const char* pString )
{
	if (!IsText())
	{
		if ( pString )
		{
			// Not text? append a null at the end.
			int nLen = (int)V_strlen( pString ) + 1;
			Put( pString, nLen * sizeof(char) );
			return;
		}
		else
		{
			PutTypeBin<char>( 0 );
		}
	}
	else if (pString)
	{
		int nTabCount = ( m_Flags & AUTO_TABS_DISABLED ) ? 0 : m_nTab;
		if ( nTabCount > 0 )
		{
			if ( WasLastCharacterCR() )
			{
				PutTabs();
			}

			const char* pEndl = strchr( pString, '\n' );
			while ( pEndl )
			{
				size_t nSize = (size_t)pEndl - (size_t)pString + sizeof(char);
				Put( pString, (int)nSize );
				pString = pEndl + 1;
				if ( *pString )
				{
					PutTabs();
					pEndl = strchr( pString, '\n' );
				}
				else
				{
					pEndl = NULL;
				}
			}
		}
		int nLen = (int)V_strlen( pString );
		if ( nLen )
		{
			Put( pString, nLen * sizeof(char) );
		}
	}
}

void CUtlBuffer::EnsureCapacityMy( int num )
{
	MEM_ALLOC_CREDIT();
	// Add one extra for the null termination
	num += 1;
	if ( m_Memory.IsExternallyAllocated() )
	{
		if ( IsGrowable() && ( m_Memory.NumAllocated() < num ) )
		{
			m_Memory.ConvertToGrowableMemory( 0 );
		}
		else
		{
			num -= 1;
		}
	}

	m_Memory.EnsureCapacity( num );
}