static const char* g_Script_vgui_init = R"script(

local _Fonts = {}
local DoCreateFont = ISurface.CreateFont;

ISurface.CreateFont <- function( name, props )
{
	if ( !("name" in props) || typeof props.name != "string" )
		throw "invalid parameter 'name'";

	local flags = 0;

	if ( "flags" in props && typeof props.flags == "integer" )
		flags = props.flags;

	if ( "italic" in props && props.italic == true )
		flags = flags | 0x001;

	if ( "underline" in props && props.underline == true )
		flags = flags | 0x002;

	if ( "strikeout" in props && props.strikeout == true )
		flags = flags | 0x004;

	if ( "symbol" in props && props.symbol == true )
		flags = flags | 0x008;

	if ( "antialias" in props && props.antialias == true )
		flags = flags | 0x010;

	if ( "gaussianblur" in props && props.gaussianblur == true )
		flags = flags | 0x020;

	if ( "rotary" in props && props.rotary == true )
		flags = flags | 0x040;

	if ( "dropshadow" in props && props.dropshadow == true )
		flags = flags | 0x080;

	if ( "additive" in props && props.additive == true )
		flags = flags | 0x100;

	if ( "outline" in props && props.outline == true )
		flags = flags | 0x200;

	if ( "custom" in props && props.custom == true )
		flags = flags | 0x400;

	if ( "bitmap" in props && props.bitmap == true )
		flags = flags | 0x800;

	local tall = 13;
	local weight = 500;
	local blur = 0;
	local scanlines = 0;

	if ( "tall" in props && typeof props.tall == "integer" )
		tall = props.tall;

	if ( "weight" in props && typeof props.weight == "integer" )
		weight = props.weight;

	if ( "blur" in props && typeof props.blur == "integer" )
		blur = props.blur;

	if ( "scanlines" in props && typeof props.scanlines == "integer" )
		scanlines = props.scanlines;

	return _Fonts[name] <- DoCreateFont( name, props.name, tall, weight, blur, scanlines, flags );
}
__Documentation.RegisterHelp( "ISurface::CreateFont", "int ISurface::CreateFont(string, handle)", "" );


local _FontTall = {}
local DoGetFont = ISurface.DoGetFont <- ISurface.GetFont;
local DoGetFontTall = ISurface.GetFontTall;

ISurface.GetFont <- function( name, sch = "ClientScheme" )
{
	if ( name in _Fonts )
		return _Fonts[name];

	local id = DoGetFont( name, sch );
	if ( id > 0 )
		_Fonts[name] <- id;

	return id;
}

ISurface.GetFontTall <- function( id )
{
	if ( id in _FontTall )
		return _FontTall[id];
	return _FontTall[id] <- DoGetFontTall( id );
}


local _Textures = {}
local DoGetTextureID = ISurface.GetTextureID;

ISurface.GetTextureID <- function( name )
{
	if ( name in _Textures )
		return _Textures[name];

	local id = DoGetTextureID( name );
	if ( id > 0 )
		_Textures[name] <- id;

	return id;
}

)script";
