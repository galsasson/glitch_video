uniform sampler2DRect src_tex_unit0;
uniform float blurAmnt;
uniform float blurAlpha;

void main( void )
{  
	vec2 st = gl_TexCoord[0].st;
	
	//vertical blur 
	//from http://www.gamerendering.com/2008/10/11/gaussian-blur-filter-shader/
	
	vec4 color;
	
	color		+= blurAlpha * 1.0 * texture2DRect(src_tex_unit0, st + vec2(0.0, blurAmnt * 4.0));
	color		+= blurAlpha * 2.0 * texture2DRect(src_tex_unit0, st + vec2(0.0, blurAmnt * 3.0));
	color		+= blurAlpha * 3.0 * texture2DRect(src_tex_unit0, st + vec2(0.0, blurAmnt * 2.0));
	color		+= blurAlpha * 4.0 * texture2DRect(src_tex_unit0, st + vec2(0.0, blurAmnt * 1.0));
		
	color		+= blurAlpha * 5.0 * texture2DRect(src_tex_unit0, st + vec2(0.0, 0.0) );
	
	color		+= blurAlpha * 4.0 * texture2DRect(src_tex_unit0, st + vec2(0.0, blurAmnt * -1.0));
	color		+= blurAlpha * 3.0 * texture2DRect(src_tex_unit0, st + vec2(0.0, blurAmnt * -2.0));
	color		+= blurAlpha * 2.0 * texture2DRect(src_tex_unit0, st + vec2(0.0, blurAmnt * -3.0));
	color		+= blurAlpha * 1.0 * texture2DRect(src_tex_unit0, st + vec2(0.0, blurAmnt * -4.0));
	
	color /= 25.0;
	gl_FragColor = color;
}
