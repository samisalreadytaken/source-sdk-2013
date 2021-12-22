//========= Mapbase - https://github.com/mapbase-source/source-sdk-2013 ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//


#include "cbase.h"
#include "tier1/utlcommon.h"

#include "inputsystem/iinputsystem.h"

#include <vgui/VGUI.h>
#include <vgui/IVGui.h>
#include <vgui/ISurface.h>
#include <vgui/IInput.h>
#include <vgui/ILocalize.h>

#include <vgui_controls/Controls.h>
#include <vgui_controls/Panel.h>
#include <vgui_controls/Label.h>
//#include <vgui_controls/Button.h>

#include <ienginevgui.h>

#include "view.h"

#include "vscript_vgui.h"
#include "vscript_vgui.nut"


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


//
//
// Exposing a new panel class (e.g. vgui::FileOpenDialog):
//
// Create 'class IScript_FileOpenDialog : IScript_Frame' with C++ bindings.
// Create 'class CScript_FileOpenDialog : FileOpenDialog' with vgui message callbacks and overrides (if needed)
// Create 'class CScript_FileOpenDialog_Helper : IScript_FileOpenDialog< CScript_FileOpenDialog >' for the script bindings
// Add new condition in CScriptVGUI::Create
//
//


using namespace vgui;
class IScriptVGUIObject;
class IScriptVGUIPanel;

static inline unsigned int IntToHandle( int i )
{
	if ( i < 0 )
		return 0;
	return static_cast<unsigned int>(i);
}

static inline int HandleToInt( unsigned int i )
{
	Assert( i <= INT_MAX );
	return static_cast<int>(i);
}

CUtlDict< HFont > g_ScriptFonts( k_eDictCompareTypeCaseSensitive );
CUtlVector< int > g_ScriptTextureIDs;
CUtlVector< IScriptVGUIObject* > g_ScriptPanels;

//--------------------------------------------------------------
//
//--------------------------------------------------------------
class CScriptSurface
{
public:
	void PlaySound( const char* sound );
	void SetColor( int r, int g, int b, int a );
	void DrawFilledRect( int x0, int y0, int width, int height );
	void DrawFilledRectFade( int x0, int y0, int width, int height, int a0, int a1, bool bHorz );
	void DrawFilledRectFastFade( int x0, int y0, int width, int height, int fadestart, int fadeend, int a0, int a1, bool bHorz );
	void DrawOutlinedRect( int x0, int y0, int width, int height );
	void DrawLine( int x0, int y0, int x1, int y1 );
	void DrawPolyLine( HSCRIPT ax, HSCRIPT ay, int count );
	void DrawOutlinedCircle( int x, int y, int radius, int segments );

	void SetTextColor( int r, int g, int b, int a );
	void SetTextPos( int x, int y );
	void DrawText( const char *text, int drawType = FONT_DRAW_DEFAULT );
	//void DrawUnicodeChar( int ch, int drawType = FONT_DRAW_DEFAULT );

	int GetFont( const char* name, const char* schema );
	//const char *GetFontName( int font );
	void SetFont( int font );
	int GetTextWidth( int font, const char* text );
	int GetFontTall( int font );
	int GetCharacterWidth( int font, int ch );

	int CreateFont( const char *customName, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags );
	bool AddCustomFontFile( const char *fontFileName );
	// bool AddBitmapFontFile( const char *fontFileName );
	int GetTextureID( char const *filename );
	int ValidateTexture( const char *filename, int hardwareFilter, bool forceReload, bool procedural );
	int GetTextureWide( int id );
	int GetTextureTall( int id );
	void SetTexture( int id );

	bool IsCursorVisible();

	void DrawTexturedRect( int x0, int y0, int width, int height );
	void DrawTexturedSubRect( int x0, int y0, int x1, int y1, float texs0, float text0, float texs1, float text1 );

	// ------------------------------------------------------------
	// Utility functions
	// ------------------------------------------------------------

	void DrawTexturedBox( int texture, int x, int y, int wide, int tall, int r, int g, int b, int a );
	void DrawColoredText( int font, int x, int y, int r, int g, int b, int a, const char *text );

} script_surface;

BEGIN_SCRIPTDESC_ROOT_NAMED( CScriptSurface, "ISurface", SCRIPT_SINGLETON "" )
	DEFINE_SCRIPTFUNC( PlaySound, "" )

	DEFINE_SCRIPTFUNC( SetColor, "" )
	DEFINE_SCRIPTFUNC( DrawFilledRect, "" )
	DEFINE_SCRIPTFUNC( DrawFilledRectFade, "" )
	DEFINE_SCRIPTFUNC( DrawFilledRectFastFade, "" )
	DEFINE_SCRIPTFUNC( DrawOutlinedRect, "" )
	DEFINE_SCRIPTFUNC( DrawLine, "" )
	DEFINE_SCRIPTFUNC( DrawPolyLine, "" )
	DEFINE_SCRIPTFUNC( DrawOutlinedCircle, "" )

	DEFINE_SCRIPTFUNC( SetTextColor, "" )
	DEFINE_SCRIPTFUNC( SetTextPos, "" )
	DEFINE_SCRIPTFUNC( DrawText, "" )

	DEFINE_SCRIPTFUNC( GetFont, "" )
	DEFINE_SCRIPTFUNC( SetFont, "" )
	DEFINE_SCRIPTFUNC( GetTextWidth, "" )
	DEFINE_SCRIPTFUNC( GetFontTall, "" )
	DEFINE_SCRIPTFUNC( GetCharacterWidth, "" )

	DEFINE_SCRIPTFUNC( CreateFont, SCRIPT_HIDE )
	DEFINE_SCRIPTFUNC( AddCustomFontFile, "" )

	DEFINE_SCRIPTFUNC( GetTextureID, "" )
	DEFINE_SCRIPTFUNC( ValidateTexture, "" )
	DEFINE_SCRIPTFUNC( GetTextureWide, "" )
	DEFINE_SCRIPTFUNC( GetTextureTall, "" )
	DEFINE_SCRIPTFUNC( SetTexture, "" )

	DEFINE_SCRIPTFUNC( IsCursorVisible, "" )

	DEFINE_SCRIPTFUNC( DrawTexturedRect, "" )
	DEFINE_SCRIPTFUNC( DrawTexturedSubRect, "" )

	DEFINE_SCRIPTFUNC( DrawTexturedBox, "" )
	DEFINE_SCRIPTFUNC( DrawColoredText, "" )
END_SCRIPTDESC()


void CScriptSurface::PlaySound( const char* sound )
{
	surface()->PlaySound(sound);
}

void CScriptSurface::SetColor( int r, int g, int b, int a )
{
	surface()->DrawSetColor( r, g, b, a );
}

void CScriptSurface::DrawFilledRect( int x0, int y0, int width, int height )
{
	surface()->DrawFilledRect( x0, y0, x0 + width, y0 + height );
}

void CScriptSurface::DrawFilledRectFade( int x0, int y0, int width, int height, int a0, int a1, bool bHorz )
{
	surface()->DrawFilledRectFade( x0, y0, x0 + width, y0 + height, a0, a1, bHorz );
}

void CScriptSurface::DrawFilledRectFastFade( int x0, int y0, int width, int height, int fadestart, int fadeend, int a0, int a1, bool bHorz )
{
	surface()->DrawFilledRectFastFade( x0, y0, x0 + width, y0 + height, fadestart, fadeend, a0, a1, bHorz );
}

void CScriptSurface::DrawOutlinedRect( int x0, int y0, int width, int height )
{
	surface()->DrawOutlinedRect( x0, y0, x0 + width, y0 + height );
}

void CScriptSurface::DrawLine( int x0, int y0, int x1, int y1 )
{
	surface()->DrawLine( x0, y0, x1, y1 );
}

void CScriptSurface::DrawPolyLine( HSCRIPT ax, HSCRIPT ay, int count )
{
	if (count < 1)
		return;

	if (count > 4096)
		count = 4096;

	int *px = (int*)stackalloc( count * sizeof(int) );
	int *py = (int*)stackalloc( count * sizeof(int) );
	ScriptVariant_t vx, vy;

	for ( int i = 0; i < count; ++i )
	{
		g_pScriptVM->GetValue( ax, i, &vx );
		g_pScriptVM->GetValue( ay, i, &vy );

		px[i] = vx.m_int;
		py[i] = vy.m_int;
	}

	surface()->DrawPolyLine( px, py, count );
}

void CScriptSurface::DrawOutlinedCircle( int x, int y, int radius, int segments )
{
	surface()->DrawOutlinedCircle( x, y, radius, segments );
}

void CScriptSurface::SetTextColor( int r, int g, int b, int a )
{
	surface()->DrawSetTextColor( r, g, b, a );
}

void CScriptSurface::SetTextPos( int x, int y )
{
	surface()->DrawSetTextPos( x, y );
}

void CScriptSurface::DrawText( const char *text, int drawType/* = FONT_DRAW_DEFAULT*/ )
{
	static wchar_t wcs[512];
	g_pVGuiLocalize->ConvertANSIToUnicode( text, wcs, sizeof(wcs) );
	surface()->DrawPrintText( wcs, wcslen(wcs), (FontDrawType_t)drawType );
}

int CScriptSurface::GetFont( const char* name, const char* schema )
{
	HFont font;
	HScheme sch;
	if ( schema )
	{
		sch = scheme()->GetScheme(schema);
	}
	else
	{
		sch = scheme()->GetDefaultScheme();
	}

	font = scheme()->GetIScheme(sch)->GetFont(name);

	if ( font == INVALID_FONT )
	{
		int idx = g_ScriptFonts.Find( name );
		if ( idx != g_ScriptFonts.InvalidIndex() )
			font = g_ScriptFonts[idx];
	}

	return HandleToInt( font );
}

void CScriptSurface::SetFont( int font )
{
	surface()->DrawSetTextFont( IntToHandle(font) );
}

int CScriptSurface::GetTextWidth( int font, const char* text )
{
	int w, t;
	static wchar_t wcs[512];
	g_pVGuiLocalize->ConvertANSIToUnicode( text, wcs, sizeof(wcs) );
	surface()->GetTextSize( IntToHandle(font), wcs, w, t );
	return w;
}

int CScriptSurface::GetFontTall( int font )
{
	return surface()->GetFontTall( IntToHandle(font) );
}

int CScriptSurface::GetCharacterWidth( int font, int ch )
{
	return surface()->GetCharacterWidth( IntToHandle(font), ch );
}

int CScriptSurface::CreateFont( const char *customName, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags )
{
	HFont font;

	int idx = g_ScriptFonts.Find( customName );
	if ( idx != g_ScriptFonts.InvalidIndex() )
	{
		font = g_ScriptFonts[idx];
	}
	else
	{
		font = surface()->CreateFont();
		g_ScriptFonts.Insert( customName, font );
	}

	surface()->SetFontGlyphSet( font, windowsFontName, tall, weight, blur, scanlines, flags );

	return HandleToInt( font );
}

bool CScriptSurface::AddCustomFontFile( const char *fontFileName )
{
	return surface()->AddCustomFontFile( NULL, fontFileName );
}

int CScriptSurface::GetTextureID( char const *filename )
{
	return surface()->DrawGetTextureId( filename );
}

// Create texture if it does not already exist
int CScriptSurface::ValidateTexture( const char *filename, int hardwareFilter, bool forceReload, bool procedural )
{
	int id = surface()->DrawGetTextureId( filename );
	if ( id <= 0 )
	{
		id = surface()->CreateNewTextureID( procedural );
		surface()->DrawSetTextureFile( id, filename, hardwareFilter, forceReload );

		// Destroy on level shutdown
		g_ScriptTextureIDs.AddToTail( id );

#ifdef _DEBUG
		char tex[MAX_PATH];
		surface()->DrawGetTextureFile( id, tex, sizeof(tex)-1 );
		if ( !V_stricmp( filename, tex ) )
		{
			Msg( "Create texture [%i]%s\n", id, filename );
		}
		else
		{
			Warning( "Create texture [%i]%s(%s)\n", id, tex, filename );
		}
#endif
	}
	return id;
}

int CScriptSurface::GetTextureWide( int id )
{
	int w, t;
	surface()->DrawGetTextureSize( id, w, t );
	return w;
}

int CScriptSurface::GetTextureTall( int id )
{
	int w, t;
	surface()->DrawGetTextureSize( id, w, t );
	return t;
}

void CScriptSurface::SetTexture( int id )
{
	surface()->DrawSetTexture( id );
}

bool CScriptSurface::IsCursorVisible()
{
	return surface()->IsCursorVisible();
}

void CScriptSurface::DrawTexturedRect( int x0, int y0, int width, int height )
{
	surface()->DrawTexturedRect( x0, y0, x0 + width, y0 + height );
}

void CScriptSurface::DrawTexturedSubRect( int x0, int y0, int x1, int y1, float texs0, float text0, float texs1, float text1 )
{
	surface()->DrawTexturedSubRect( x0, y0, x1, y1, texs0, text0, texs1, text1 );
}

void CScriptSurface::DrawTexturedBox( int texture, int x, int y, int wide, int tall, int r, int g, int b, int a )
{
	surface()->DrawSetColor( r, g, b, a );
	surface()->DrawSetTexture( texture );
	surface()->DrawTexturedRect( x, y, x + wide, y + tall );
}

void CScriptSurface::DrawColoredText( int font, int x, int y, int r, int g, int b, int a, const char *text )
{
	static wchar_t wcs[512];
	g_pVGuiLocalize->ConvertANSIToUnicode( text, wcs, sizeof(wcs) );

	surface()->DrawSetTextFont( IntToHandle(font) );
	surface()->DrawSetTextColor( r, g, b, a );
	surface()->DrawSetTextPos( x, y );
	surface()->DrawPrintText( wcs, wcslen(wcs) );
}


//==============================================================
//==============================================================


#define DECLARE_SCRIPTVGUI_HELPER( panelClass )\
	void Create( VPANEL parent, const char *panelName ) override\
	{\
		Assert(!_base);\
		_base = new panelClass( ipanel()->GetPanel( parent, GetControlsModuleName() ), panelName );\
		_vpanel = _base->GetVPanel();\
	}

#define DECLARE_SCRIPTVGUI_HELPER_DEFAULT_TEXT( panelClass )\
	void Create( VPANEL parent, const char *panelName ) override\
	{\
		Assert(!_base);\
		_base = new panelClass( ipanel()->GetPanel( parent, GetControlsModuleName() ), panelName, (const char*)NULL );\
		_vpanel = _base->GetVPanel();\
	}


#define DEFAULT_VGUI_PARENT enginevgui->GetPanel( PANEL_INGAMESCREENS )


class IScriptVGUIObject
{
public:
#ifdef _DEBUG
	virtual const char *GetName() = 0;
#endif
	//-----------------------------------------------------
	// Free the VGUI panel and script instance.
	//-----------------------------------------------------
	virtual void Destroy() = 0;

	//-----------------------------------------------------
	// Create new panel object.
	// Do NOT register C++ created panels!
	//-----------------------------------------------------
	virtual void Create(VPANEL parent, const char *panelName) = 0;

public:
	inline VPANEL GetVPanel() { return _vpanel; }
	inline HSCRIPT GetScriptInstance() { return m_hScriptInstance; }

	VPANEL _vpanel;
	HSCRIPT m_hScriptInstance;
};

BEGIN_SCRIPTDESC_ROOT( IScriptVGUIObject, "" )
END_SCRIPTDESC()


static void ResolveChildren( VPANEL panel )
{
	CUtlVector< VPANEL > &children = ipanel()->GetChildren( panel );
	FOR_EACH_VEC_BACK( children, i )
	{
		VPANEL child = children[i];
		FOR_EACH_VEC( g_ScriptPanels, j )
		{
			IScriptVGUIObject *obj = g_ScriptPanels[j];
			if ( obj->GetVPanel() == child )
			{
				if ( ipanel()->IsAutoDeleteSet(child) )
				{
#ifdef _DEBUG
					Msg( "ResolveChildren: '%s' (autodelete)\n", obj->GetName() );
#endif
					if ( g_pScriptVM )
						g_pScriptVM->RemoveInstance( obj->m_hScriptInstance );
					obj->m_hScriptInstance = NULL;
					obj->_vpanel = NULL;
					ResolveChildren( child );
				}
				else
				{
#ifdef _DEBUG
					Msg( "ResolveChildren: '%s'\n", obj->GetName() );
#endif
					ipanel()->SetParent( child, DEFAULT_VGUI_PARENT );
				}
				break;
			}
		}
	}
}

template <class T>
class CScriptVGUIObject : public IScriptVGUIObject
{
public:
	T *_base;

	CScriptVGUIObject() : _base(0)
	{
		_vpanel = 0;
		m_hScriptInstance = 0;
	}

	~CScriptVGUIObject()
	{
		Destroy();
	}

	void Destroy()
	{
		if ( GetVPanel() )
		{
			Assert( _base );
#ifdef _DEBUG
			Msg( "Destroy panel '%s'\n", _base->GetName() );
#endif
			ResolveChildren( _vpanel );
			_base->Shutdown(); // IScriptVGUIPanel
			_base->MarkForDeletion();
			_vpanel = NULL;
		}

		if ( m_hScriptInstance )
		{
			if ( g_pScriptVM )
				g_pScriptVM->RemoveInstance( m_hScriptInstance );
			m_hScriptInstance = NULL;
		}
	}

	void CreateFromScript( HSCRIPT parent, const char *panelName )
	{
		Create( (VPANEL)NULL, panelName && *panelName ? panelName : NULL );

		if ( parent )
		{
			IScriptVGUIObject *obj = (IScriptVGUIObject *)g_pScriptVM->GetInstanceValue( parent, ::GetScriptDesc( (IScriptVGUIObject *)0 ) );
			if ( obj )
			{
				_base->SetParent( obj->GetVPanel() );
				return;
			}

			Assert( !"invalid parent" );
		}

		_base->SetParent( DEFAULT_VGUI_PARENT );
	}
};

//--------------------------------------------------------------
//--------------------------------------------------------------

template <class T>
class IScript_Panel : public CScriptVGUIObject<T>
{
public:
	void MakeReadyForUse()
	{
		_base->MakeReadyForUse();
	}

	const char *GetName()
	{
		return _base->GetName();
	}

	void SetAutoDelete( bool i )
	{
		_base->SetAutoDelete(i);
	}

	//-----------------------------------------------------
	// Get script created parent
	//-----------------------------------------------------
	HSCRIPT	GetParent()
	{
		VPANEL parent = ipanel()->GetParent( GetVPanel() );
		if ( !parent )
			return NULL;

		FOR_EACH_VEC( g_ScriptPanels, i )
		{
			IScriptVGUIObject* obj = g_ScriptPanels[i];
			if ( obj->GetVPanel() == parent )
			{
				// My parent can't be invalid.
				Assert( obj->GetScriptInstance() );

				return obj->GetScriptInstance();
			}
		}

		// My parent wasn't registered.
		Assert( parent == DEFAULT_VGUI_PARENT );

		return NULL;
	}

	//-----------------------------------------------------
	// Set script created parent
	//-----------------------------------------------------
	void SetParent( HSCRIPT parent )
	{
		if ( !parent )
		{
			_base->SetParent( DEFAULT_VGUI_PARENT );
			return;
		}

		IScriptVGUIObject *obj = (IScriptVGUIObject *)g_pScriptVM->GetInstanceValue( parent, ::GetScriptDesc( (IScriptVGUIObject *)0 ) );
		if ( obj )
		{
			_base->SetParent( obj->GetVPanel() );
			return;
		}

		Assert( !"invalid parent" );
	}

	//-----------------------------------------------------
	// Get script created children
	//-----------------------------------------------------
	void GetChildren( HSCRIPT arr )
	{
		CUtlVector< VPANEL > &children = ipanel()->GetChildren( GetVPanel() );
		FOR_EACH_VEC( children, i )
		{
			VPANEL child = children[i];
			FOR_EACH_VEC( g_ScriptPanels, j )
			{
				IScriptVGUIObject* obj = g_ScriptPanels[j];
				if ( obj->GetVPanel() == child )
				{
					g_pScriptVM->ArrayAppend( arr, obj->GetScriptInstance() );
					break;
				}
			}
		}
	}

	void GetPos( HSCRIPT out )
	{
		int x, y;
		ipanel()->GetPos( GetVPanel(), x, y );
		g_pScriptVM->SetValue( out, (ScriptVariant_t)0, x );
		g_pScriptVM->SetValue( out, 1, y );
	}

	void SetPos( int x, int y )
	{
		_base->SetPos( x, y );
	}

	void SetZPos( int i )
	{
		ipanel()->SetZPos( GetVPanel(), i );
	}

	int GetZPos()
	{
		return ipanel()->GetZPos( GetVPanel() );
	}

	void SetSize( int w, int t )
	{
		_base->SetSize( w, t );
	}

	void SetWide( int i )
	{
		_base->SetWide(i);
	}

	int GetWide()
	{
		return _base->GetWide();
	}

	void SetTall( int i )
	{
		_base->SetTall(i);
	}

	int GetTall()
	{
		return _base->GetTall();
	}

	int GetAlpha()
	{
		return _base->GetAlpha();
	}

	void SetAlpha( int i )
	{
		_base->SetAlpha( i );
	}

	void SetVisible( bool i )
	{
		ipanel()->SetVisible( GetVPanel(), i );
	}

	bool IsVisible()
	{
		return ipanel()->IsVisible( GetVPanel() );
	}

	void LocalToScreen( int x, int y, HSCRIPT out )
	{
		int px, py;
		ipanel()->GetAbsPos( GetVPanel(), px, py );

		g_pScriptVM->SetValue( out, (ScriptVariant_t)0, x + px );
		g_pScriptVM->SetValue( out, 1, y + py );
	}

	void ScreenToLocal( int x, int y, HSCRIPT out )
	{
		int px, py;
		ipanel()->GetAbsPos( GetVPanel(), px, py );

		g_pScriptVM->SetValue( out, (ScriptVariant_t)0, x - px );
		g_pScriptVM->SetValue( out, 1, y - py );
	}

	bool IsWithin( int x, int y )
	{
		return _base->IsWithin( x, y );
	}

	void AddTickSignal( int i )
	{
		ivgui()->AddTickSignal( GetVPanel(), i );
	}

	void RequestFocus()
	{
		_base->RequestFocus();
	}

	void SetEnabled( bool i )
	{
		_base->SetEnabled(i);
	}

	bool IsEnabled()
	{
		return _base->IsEnabled();
	}

	void SetPaintEnabled( bool i )
	{
		_base->SetPaintEnabled(i);
	}

	void SetPaintBackgroundEnabled( bool i )
	{
		_base->SetPaintBackgroundEnabled(i);
	}

	void SetPaintBorderEnabled( bool i )
	{
		_base->SetPaintBorderEnabled(i);
	}

	// 0 for normal(opaque), 1 for single texture from Texture1, and 2 for rounded box w/ four corner textures
	void SetPaintBackgroundType( int i )
	{
		_base->SetPaintBackgroundType(i);
	}

	void SetFgColor( int r, int g, int b, int a )
	{
		_base->SetFgColor( Color( r, g, b, a ) );
	}

	void SetBgColor( int r, int g, int b, int a )
	{
		_base->SetBgColor( Color( r, g, b, a ) );
	}

	bool IsCursorOver()
	{
		return _base->IsCursorOver();
	}

	void SetMouseInputEnabled( bool i )
	{
		_base->SetMouseInputEnabled(i);
	}

	void SetKeyBoardInputEnabled( bool i )
	{
		_base->SetKeyBoardInputEnabled(i);
	}

	// -----------------------
	// Drawing utility
	// -----------------------
	//void SetRoundedCorners( int cornerFlags )
	//{
	//	_base->SetRoundedCorners( cornerFlags & 0xff );
	//}

	void DrawBox( int x, int y, int wide, int tall, int r, int g, int b, int a, bool hollow = false )
	{
		_base->DrawBox( x, y, wide, tall, Color(r, g, b, a), 1.0f, hollow );
	}

	void DrawBoxFade( int x, int y, int wide, int tall, int r, int g, int b, int a, int alpha0, int alpha1, bool bHorizontal, bool hollow = false )
	{
		_base->DrawBoxFade( x, y, wide, tall, Color(r, g, b, a), 1.0f, alpha0, alpha1, bHorizontal, hollow );
	}
#if 0
	// -----------------------
	// drag drop
	// -----------------------
	void SetDragEnabled( bool i )
	{
		_base->SetDragEnabled(i);
	}

	bool IsDragEnabled()
	{
		return _base->IsDragEnabled();
	}

	void SetDropEnabled( bool i )
	{
		_base->SetDropEnabled( i, 0.0f );
	}

	bool IsDropEnabled()
	{
		return _base->IsDropEnabled();
	}

	void SetShowDragHelper( int i )
	{
		_base->SetShowDragHelper(i);
	}

	int GetDragStartTolerance()
	{
		return _base->GetDragStartTolerance();
	}

	void SetDragStartTolerance( int i )
	{
		_base->SetDragSTartTolerance(i);
	}
#endif
};

#define DEFINE_VGUI_SCRIPTFUNC_Panel()\
	DEFINE_SCRIPTFUNC( Destroy, "" )\
	DEFINE_SCRIPTFUNC( MakeReadyForUse, "" )\
	DEFINE_SCRIPTFUNC( GetName, "" )\
	DEFINE_SCRIPTFUNC( SetAutoDelete, "" )\
\
	DEFINE_SCRIPTFUNC( GetParent, "" )\
	DEFINE_SCRIPTFUNC( SetParent, "" )\
	DEFINE_SCRIPTFUNC( GetChildren, "" )\
\
	DEFINE_SCRIPTFUNC( GetPos, "" )\
	DEFINE_SCRIPTFUNC( SetPos, "" )\
	DEFINE_SCRIPTFUNC( GetZPos, "" )\
	DEFINE_SCRIPTFUNC( SetZPos, "" )\
\
	DEFINE_SCRIPTFUNC( SetSize, "" )\
	DEFINE_SCRIPTFUNC( GetWide, "" )\
	DEFINE_SCRIPTFUNC( SetWide, "" )\
\
	DEFINE_SCRIPTFUNC( GetTall, "" )\
	DEFINE_SCRIPTFUNC( SetTall, "" )\
\
	DEFINE_SCRIPTFUNC( GetAlpha, "" )\
	DEFINE_SCRIPTFUNC( SetAlpha, "" )\
\
	DEFINE_SCRIPTFUNC( SetVisible, "" )\
	DEFINE_SCRIPTFUNC( IsVisible, "" )\
\
	DEFINE_SCRIPTFUNC( LocalToScreen, "" )\
	DEFINE_SCRIPTFUNC( ScreenToLocal, "" )\
	DEFINE_SCRIPTFUNC( IsWithin, "" )\
\
	DEFINE_SCRIPTFUNC( AddTickSignal, "" )\
	DEFINE_SCRIPTFUNC( RequestFocus, "" )\
\
	DEFINE_SCRIPTFUNC( SetEnabled, "" )\
	DEFINE_SCRIPTFUNC( IsEnabled, "" )\
	DEFINE_SCRIPTFUNC( SetPaintEnabled, "" )\
	DEFINE_SCRIPTFUNC( SetPaintBackgroundEnabled, "" )\
	DEFINE_SCRIPTFUNC( SetPaintBorderEnabled, "" )\
	DEFINE_SCRIPTFUNC( SetPaintBackgroundType, "" )\
\
	DEFINE_SCRIPTFUNC( SetFgColor, "" )\
	DEFINE_SCRIPTFUNC( SetBgColor, "" )\
\
	DEFINE_SCRIPTFUNC( IsCursorOver, "" )\
\
	DEFINE_SCRIPTFUNC( SetMouseInputEnabled, "" )\
	DEFINE_SCRIPTFUNC( SetKeyBoardInputEnabled, "" )\
\
	DEFINE_SCRIPTFUNC( DrawBox, "" )\
	DEFINE_SCRIPTFUNC( DrawBoxFade, "" )\

//--------------------------------------------------------------
//--------------------------------------------------------------

template <class T>
class IScript_Label : public IScript_Panel<T>
{
public:
	void SetText( const char *text )
	{
		static wchar_t wcs[512];
		g_pVGuiLocalize->ConvertANSIToUnicode( text, wcs, sizeof(wcs) );
		_base->SetText( wcs );
	}

	void SetFont( int i )
	{
		_base->SetFont( IntToHandle(i) );
	}

	void SetAllCaps( bool i )
	{
		_base->SetAllCaps(i);
	}

	void SetWrap( bool i )
	{
		_base->SetWrap(i);
	}

	void SetContentAlignment( int i )
	{
		_base->SetContentAlignment( (Label::Alignment)i );
	}

	void SetTextInset( int x, int y )
	{
		_base->SetTextInset( x, y );
	}

	void SizeToContents()
	{
		_base->SizeToContents();
	}

	//void SetAssociatedControl( HSCRIPT control )
};

#define DEFINE_VGUI_SCRIPTFUNC_Label()\
	DEFINE_VGUI_SCRIPTFUNC_Panel()\
	DEFINE_SCRIPTFUNC( SetText, "" )\
	DEFINE_SCRIPTFUNC( SetFont, "" )\
	DEFINE_SCRIPTFUNC( SetAllCaps, "" )\
	DEFINE_SCRIPTFUNC( SetWrap, "" )\
	DEFINE_SCRIPTFUNC( SetContentAlignment, "" )\
	DEFINE_SCRIPTFUNC( SetTextInset, "" )\
	DEFINE_SCRIPTFUNC( SizeToContents, "" )\

//--------------------------------------------------------------
//--------------------------------------------------------------
/*
template <class T>
class IScript_Button : public IScript_Label<T>
{
public:
	void SetButtonActivationType( int activationType )
	{
		_base->SetButtonActivationType( (Button::ActivationType_t)activationType );
	}

	bool IsArmed()
	{
		return _base->IsArmed();
	}

	void SetArmed( bool state )
	{
		_base->SetArmed(state);
	}

	bool IsSelected()
	{
		return _base->IsSelected();
	}

	void SetSelected( bool state )
	{
		_base->SetSelected(state);
	}

	bool IsDepressed()
	{
		return _base->IsDepressed();
	}

	void ForceDepressed( bool state )
	{
		_base->ForceDepressed(state);
	}

	bool IsBlinking()
	{
		return _base->IsBlinking();
	}

	void SetBlink( bool state )
	{
		_base->SetBlink(state);
	}

	void SetUseCaptureMouse( bool state )
	{
		_base->SetUseCaptureMouse( state );
	}

	void SetMouseClickEnabled( int code, bool state )
	{
		_base->SetMouseClickEnabled( (MouseCode)code, state );
	}

	bool IsMouseClickEnabled( int code )
	{
		return _base->IsMouseClickEnabled( (MouseCode)code );
	}

	void SetButtonBorderEnabled( bool state )
	{
		_base->SetButtonBorderEnabled( state );
	}

	void SetDefaultColor( int fr, int fg, int fb, int fa, int br, int bg, int bb, int ba )
	{
		_base->SetDefaultColor( Color(fr, fg, fb, fa), Color(br, bg, bb, ba) );
	}

	void SetArmedColor( int fr, int fg, int fb, int fa, int br, int bg, int bb, int ba )
	{
		_base->SetArmedColor( Color(fr, fg, fb, fa), Color(br, bg, bb, ba) );
	}

	void SetSelectedColor( int fr, int fg, int fb, int fa, int br, int bg, int bb, int ba )
	{
		_base->SetSelectedColor( Color(fr, fg, fb, fa), Color(br, bg, bb, ba) );
	}

	void SetDepressedColor( int fr, int fg, int fb, int fa, int br, int bg, int bb, int ba )
	{
		_base->SetDepressedColor( Color(fr, fg, fb, fa), Color(br, bg, bb, ba) );
	}

	void SetBlinkColor( int r, int g, int b, int a )
	{
		_base->SetBlinkColor( Color(r, g, b, a) );
	}

	void SetArmedSound( const char *sound )
	{
		_base->SetReleasedSound( sound );
	}

	void SetDepressedSound( const char *sound )
	{
		_base->SetReleasedSound( sound );
	}

	void SetReleasedSound( const char *sound )
	{
		_base->SetReleasedSound( sound );
	}
};

#define DEFINE_VGUI_SCRIPTFUNC_Button()\
	DEFINE_VGUI_SCRIPTFUNC_Label()\
	DEFINE_SCRIPTFUNC( SetButtonActivationType, "" )\
	DEFINE_SCRIPTFUNC( IsArmed, "" )\
	DEFINE_SCRIPTFUNC( SetArmed, "" )\
	DEFINE_SCRIPTFUNC( IsSelected, "" )\
	DEFINE_SCRIPTFUNC( SetSelected, "" )\
	DEFINE_SCRIPTFUNC( IsDepressed, "" )\
	DEFINE_SCRIPTFUNC( ForceDepressed, "" )\
	DEFINE_SCRIPTFUNC( IsBlinking, "" )\
	DEFINE_SCRIPTFUNC( SetBlink, "" )\
	DEFINE_SCRIPTFUNC( SetUseCaptureMouse, "" )\
	DEFINE_SCRIPTFUNC( SetMouseClickEnabled, "" )\
	DEFINE_SCRIPTFUNC( IsMouseClickEnabled, "" )\
	DEFINE_SCRIPTFUNC( SetButtonBorderEnabled, "" )\
	DEFINE_SCRIPTFUNC( SetDefaultColor, "" )\
	DEFINE_SCRIPTFUNC( SetArmedColor, "" )\
	DEFINE_SCRIPTFUNC( SetSelectedColor, "" )\
	DEFINE_SCRIPTFUNC( SetDepressedColor, "" )\
	DEFINE_SCRIPTFUNC( SetBlinkColor, "" )\
	DEFINE_SCRIPTFUNC( SetArmedSound, "" )\
	DEFINE_SCRIPTFUNC( SetDepressedSound, "" )\
	DEFINE_SCRIPTFUNC( SetReleasedSound, "" )
*/
//--------------------------------------------------------------
//--------------------------------------------------------------


//==============================================================
//==============================================================


// HACKHACK: In singleplayer game restore, panel destructor is called after
// the script VM is restarted and the HSCRIPT members are released.
// Use common interface to nullify members in IScriptVGUIObject::Destroy()
class IScriptVGUIPanel
{
public:
	void Shutdown() {}
};


#define SetHScript( var, val ) \
	if ( var && g_pScriptVM ) \
		g_pScriptVM->ReleaseScript( var ); \
	var = val;

//--------------------------------------------------------
// C++ objects for vgui overrides and messages
//--------------------------------------------------------
class CScript_Panel : public Panel, public IScriptVGUIPanel
{
	typedef Panel BaseClass;

public:
	HSCRIPT m_hfnPaint;
	HSCRIPT m_hfnPaintBackground;

	HSCRIPT m_hfnOnTick;
	HSCRIPT m_hfnOnScreenSizeChanged;

public:
	CScript_Panel( Panel *parent, const char *name ) :
		BaseClass( parent, name ),

		m_hfnPaint(NULL),
		m_hfnPaintBackground(NULL),

		m_hfnOnTick(NULL),
		m_hfnOnScreenSizeChanged(NULL)
	{}

	void Shutdown()
	{
		SetHScript( m_hfnPaint, NULL );
		SetHScript( m_hfnPaintBackground, NULL );

		SetHScript( m_hfnOnTick, NULL );
		SetHScript( m_hfnOnScreenSizeChanged, NULL );
	}

public:
	void Paint()
	{
		g_pScriptVM->ExecuteFunction( m_hfnPaint, NULL, 0, NULL, NULL, true );
	}

	void PaintBackground()
	{
		if ( m_hfnPaintBackground )
		{
			g_pScriptVM->ExecuteFunction( m_hfnPaintBackground, NULL, 0, NULL, NULL, true );
		}
		else
		{
			BaseClass::PaintBackground();
		}
	}

	void OnTick()
	{
		BaseClass::OnTick();
		g_pScriptVM->ExecuteFunction( m_hfnOnTick, NULL, 0, NULL, NULL, true );
	}

	void OnScreenSizeChanged( int oldwide, int oldtall )
	{
		BaseClass::OnScreenSizeChanged( oldwide, oldtall );

		if ( m_hfnOnScreenSizeChanged )
		{
			ScriptVariant_t args[2] = { oldwide, oldtall };
			g_pScriptVM->ExecuteFunction( m_hfnOnScreenSizeChanged, args, 2, NULL, NULL, true );
		}
	}

public:
	void SetCallback( const char* cb, HSCRIPT fn )
	{
#define Check(s)\
	if ( FStrEq( cb, #s ) )\
	{\
		SetHScript( m_hfn##s, fn );\
		return;\
	}

		Check( Paint );
		Check( PaintBackground );

		Check( OnTick );
		Check( OnScreenSizeChanged );

		g_pScriptVM->RaiseException("invalid callback");

#undef Check
	}
};

//--------------------------------------------------------------
//--------------------------------------------------------------

class CScript_Label : public Label, public IScriptVGUIPanel
{
public:
	CScript_Label(Panel *parent, const char *name, const char *text)
		: Label( parent, name, text )
	{}
};

//--------------------------------------------------------------
//--------------------------------------------------------------


//==============================================================
//==============================================================


//--------------------------------------------------------
// Script objects
//--------------------------------------------------------
class CScript_Panel_Helper : public IScript_Panel< CScript_Panel >
{
	DECLARE_SCRIPTVGUI_HELPER( CScript_Panel )

public:
	void SetCallback( const char* cb, HSCRIPT fn )
	{
		_base->SetCallback( cb, fn );
	}
};

BEGIN_SCRIPTDESC_NAMED( CScript_Panel_Helper, IScriptVGUIObject, "Panel", "" )
	DEFINE_VGUI_SCRIPTFUNC_Panel()
	DEFINE_SCRIPTFUNC( SetCallback, "" )
END_SCRIPTDESC();

//--------------------------------------------------------------
//--------------------------------------------------------------

class CScript_Label_Helper : public IScript_Label< CScript_Label >
{
	DECLARE_SCRIPTVGUI_HELPER_DEFAULT_TEXT( CScript_Label )

public:
};

BEGIN_SCRIPTDESC_NAMED( CScript_Label_Helper, IScriptVGUIObject, "Label", "" )
	DEFINE_VGUI_SCRIPTFUNC_Label()
END_SCRIPTDESC()

//--------------------------------------------------------------
//--------------------------------------------------------------


//==============================================================
//==============================================================


class CScriptVGUI : public CAutoGameSystem
{
public:
	void LevelShutdownPostEntity();

public:
	HSCRIPT Create( const char* type, HSCRIPT parent, const char* name );

} script_vgui;

BEGIN_SCRIPTDESC_ROOT_NAMED( CScriptVGUI, "IVGui", SCRIPT_SINGLETON"" )
	DEFINE_SCRIPTFUNC( Create, "" )
END_SCRIPTDESC()


HSCRIPT CScriptVGUI::Create( const char* type, HSCRIPT parent, const char* name )
{
#define Check( _name )\
	if ( FStrEq( type, #_name ) )\
	{\
		CScript_##_name##_Helper *helper = new CScript_##_name##_Helper;\
		helper->CreateFromScript( (HSCRIPT)parent, name );\
		g_ScriptPanels.AddToTail( helper );\
		HSCRIPT ret = g_pScriptVM->RegisterInstance( helper );\
		helper->m_hScriptInstance = ret;\
		return ret;\
	}

	Check( Panel );
	Check( Label );
	//Check( Button );

	g_pScriptVM->RaiseException("invalid vgui panel type");
	return NULL;

#undef Check
}

void CScriptVGUI::LevelShutdownPostEntity()
{
	FOR_EACH_VEC( g_ScriptPanels, i )
	{
		g_ScriptPanels[i]->Destroy();
	}
	g_ScriptPanels.PurgeAndDeleteElements();

	FOR_EACH_VEC( g_ScriptTextureIDs, i )
	{
#ifdef _DEBUG
		char tex[MAX_PATH];
		surface()->DrawGetTextureFile( g_ScriptTextureIDs[i], tex, sizeof(tex)-1 );
		Msg( "Destroy texture [%i]%s\n", g_ScriptTextureIDs[i], tex );
#endif
		surface()->DestroyTextureID( g_ScriptTextureIDs[i] );
	}
	g_ScriptTextureIDs.Purge();
}


class CScriptIInput
{
public:
#if 0
	int GetButtonBits()
	{
		return ::input->GetButtonBits(0);
	}

	void ClearInputButton( int i )
	{
		return ::input->ClearInputButton(i);
	}
#endif
	void GetCursorPos( HSCRIPT out )
	{
		int x, y;
		vgui::input()->GetCursorPos( x, y );
		g_pScriptVM->SetValue( out, (ScriptVariant_t)0, x );
		g_pScriptVM->SetValue( out, 1, y );
	}

	void SetCursorPos( int x, int y )
	{
		vgui::input()->SetCursorPos( x, y );
	}

	int GetAnalogValue( int code )
	{
		return inputsystem->GetAnalogValue( (AnalogCode_t)code );
	}

	int GetAnalogDelta( int code )
	{
		return inputsystem->GetAnalogDelta( (AnalogCode_t)code );
	}

	bool IsButtonDown( int code )
	{
		return inputsystem->IsButtonDown( (ButtonCode_t)code );
	}

	int GetButtonCodeForBind( const char *bind )
	{
		return inputsystem->StringToButtonCode( bind );
	}

	const char *LookupBinding( const char *binding )
	{
		return engine->Key_LookupBinding( binding );
	}

	const char *BindingForKey( int code )
	{
		return engine->Key_BindingForKey( (ButtonCode_t)code );
	}
#if 0
	const char *GetIMELanguageShortCode()
	{
		static char ret[5];
		wchar_t get[5];
		get[0] = L'\0';
		vgui::input()->GetIMELanguageShortCode( get, wcslen(get) );
		g_pVGuiLocalize->ConvertUnicodeToANSI( get, ret, sizeof(ret) );
		return ret;
	}
#endif
} script_input;

BEGIN_SCRIPTDESC_ROOT_NAMED( CScriptIInput, "IInput", SCRIPT_SINGLETON"" )
	DEFINE_SCRIPTFUNC( GetCursorPos, "" )
	DEFINE_SCRIPTFUNC( SetCursorPos, "" )

	DEFINE_SCRIPTFUNC( GetAnalogValue, "" )
	DEFINE_SCRIPTFUNC( GetAnalogDelta, "" )
	DEFINE_SCRIPTFUNC( IsButtonDown, "" )

	DEFINE_SCRIPTFUNC( GetButtonCodeForBind, "" )
	DEFINE_SCRIPTFUNC( LookupBinding, "" )
	DEFINE_SCRIPTFUNC( BindingForKey, "" )
END_SCRIPTDESC()


//==============================================================
//==============================================================


//-----------------------------------------------------------------------------
// Get world position in screen space [0,1]. Return true if on screen.
//-----------------------------------------------------------------------------
inline bool WorldToScreen( const Vector &pos, int &ix, int &iy )
{
	int scrw, scrh;
	surface()->GetScreenSize( scrw, scrh );

	const VMatrix &worldToScreen = engine->WorldToScreenMatrix();
	bool bOnScreen;

	// VMatrix * Vector (position projective)
	vec_t w = worldToScreen[3][0] * pos[0] + worldToScreen[3][1] * pos[1] + worldToScreen[3][2] * pos[2] + worldToScreen[3][3];
	vec_t fx = worldToScreen[0][0] * pos[0] + worldToScreen[0][1] * pos[1] + worldToScreen[0][2] * pos[2] + worldToScreen[0][3];
	vec_t fy = worldToScreen[1][0] * pos[0] + worldToScreen[1][1] * pos[1] + worldToScreen[1][2] * pos[2] + worldToScreen[1][3];

	if ( w < 0.001f )
	{
		fx *= 1e5f;
		fy *= 1e5f;
		bOnScreen = false;
	}
	else
	{
		w = 1.0f / w;
		fx *= w;
		fy *= w;
		bOnScreen = true;
	}

	ix = (int)( scrw * 0.5f * ( 1.0f + fx ) + 0.5f );
	iy = (int)( scrh * 0.5f * ( 1.0f - fy ) + 0.5f );

	return bOnScreen;
}

//-----------------------------------------------------------------------------
// Get screen pixel position [0,1] in world space.
//-----------------------------------------------------------------------------
inline void ScreenToWorld( int x, int y, Vector &out )
{
	int scrw, scrh;
	surface()->GetScreenSize( scrw, scrh );
	float scrx = (float)x / (float)scrw;
	float scry = (float)y / (float)scrh;

	vec_t tmp[2];
	tmp[0] = 2.0f * scrx - 1.0f;
	tmp[1] = 1.0f - 2.0f * scry;
	//tmp[2] = 1.0f;
	//tmp[3] = 1.0f;

	VMatrix screenToWorld;
	MatrixInverseGeneral( engine->WorldToScreenMatrix(), screenToWorld );

	// VMatrix * Vector (position projective)
	vec_t iw = 1.0f / ( screenToWorld[3][0] * tmp[0] + screenToWorld[3][1] * tmp[1] + screenToWorld[3][2] + screenToWorld[3][3] );
	out[0] = iw * ( screenToWorld[0][0] * tmp[0] + screenToWorld[0][1] * tmp[1] + screenToWorld[0][2] + screenToWorld[0][3] );
	out[1] = iw * ( screenToWorld[1][0] * tmp[0] + screenToWorld[1][1] * tmp[1] + screenToWorld[1][2] + screenToWorld[1][3] );
	out[2] = iw * ( screenToWorld[2][0] * tmp[0] + screenToWorld[2][1] * tmp[1] + screenToWorld[2][2] + screenToWorld[2][3] );
}


static bool ScriptWorldToScreen( const Vector &pos, HSCRIPT out )
{
	int ix, iy;
	bool r = WorldToScreen( pos, ix, iy );

	g_pScriptVM->SetValue( out, (ScriptVariant_t)0, ix );
	g_pScriptVM->SetValue( out, 1, iy );
	return r;
}

static const Vector& ScriptScreenToWorld( int x, int y )
{
	static Vector out;
	ScreenToWorld( x, y, out );
	return out;
}

static const Vector& ScreenToRay( int x, int y )
{
	static Vector out;
	ScreenToWorld( x, y, out );
	VectorSubtract( out, CurrentViewOrigin(), out );
	VectorNormalize( out );
	return out;
}

//-----------------------------------------------------------------------------
// Get world position normalised in screen space. Return true if on screen.
//-----------------------------------------------------------------------------
int ScreenTransform( const Vector& point, Vector& screen );
static bool ScriptScreenTransform( const Vector &pos, HSCRIPT out )
{
	Vector v;
	bool r = ScreenTransform( pos, v );
	float x = 0.5f * ( 1.0f + v[0] );
	float y = 0.5f * ( 1.0f - v[1] );

	g_pScriptVM->SetValue( out, (ScriptVariant_t)0, x );
	g_pScriptVM->SetValue( out, 1, y );
	return !r;
}


void RegisterScriptVGUI()
{
	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptWorldToScreen, "WorldToScreen", "Get world position in screen space [0,1]. Return true if on screen." );
	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptScreenToWorld, "ScreenToWorld", "Get screen pixel position [0,1] in world space." );
	ScriptRegisterFunction( g_pScriptVM, ScreenToRay, "Get a ray from screen pixel position to world space." );
	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptScreenTransform, "ScreenTransform", "Get world position normalised in screen space. Return true if on screen." );

	g_pScriptVM->RegisterInstance( &script_surface, "surface" );
	g_pScriptVM->RegisterInstance( &script_input, "input" );
	g_pScriptVM->RegisterInstance( &script_vgui, "vgui" );

	g_pScriptVM->Run( g_Script_vgui_init );
}
