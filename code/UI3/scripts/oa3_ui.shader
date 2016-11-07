grad
{
	nopicmip
	{
		map ui/assets5/gradient32.tga
		blendfunc blend
		rgbGen Vertex
		tcMod stretch square 6.3 0 0 1 
	}
}

gradhorz
{
	nopicmip
	{
		map ui/assets5/gradienthorz32.tga
		blendfunc blend
		rgbGen Vertex
		tcMod stretch square 6.3 0 0 1 
	}
}


ui/hud/cornerbarsflipped
{
	nopicmip
	{
		map ui/hud/cornerbars.tga
		blendfunc blend
		rgbGen Vertex
		tcMod scale -1 1
	}
}

gradhorzv
{
	nopicmip
	{
		map ui/assets5/gradienthorz32.tga
		blendfunc blend
		rgbGen Vertex
		tcMod stretch square 6.3 0 0 1 
		tcMod scale -1 -1
	}
}

gradv
{
	nopicmip
	{
		map ui/assets5/gradient32.tga
		blendfunc blend
		rgbGen Vertex
		tcMod stretch square 6.3 0 0 1 
		tcMod scale -1 -1
	}
}

gradv
{
	nopicmip
	{
		map ui/assets5/gradient32.tga
		blendfunc blend
		rgbGen Vertex
		tcMod stretch square 6.3 0 0 1 
		tcMod scale -1 -1
	}
}


emptykey
{
	nopicmip
	{
		map ui/assets5/emptykey.tga
		rgbGen Vertex
		tcMod scale 1 0.625
	}
	{
		map ui/assets5/emptykey.tga
		blendfunc blend
		rgbGen identityLighting
		tcMod scale 1 0.625
	}
}


consoleOLD
{
	nopicmip
	{
		map ui/assets5/gradient32.tga
		blendfunc blend
		rgbGen const ( 0.17451 0.135294 0.166667 )
		tcMod stretch square 6.3 0 0 1 
	}
}

azurebuttonforced
{
	nopicmip
	{
		map ui/assets5/buttonmask.tga
		blendfunc add
		depthWrite
		alphaFunc GE128
		detail
	}
	{
		map ui/assets5/techyscroll3.tga
		blendfunc blend
		rgbGen const ( 0.254902 0.290196 0.427451 )
		tcMod scroll 0 0.2
		depthFunc equal
	}
	{
		map ui/assets5/techyscroll3.tga
		blendfunc blend
		rgbGen const ( 0.27451 0.34902 0.423529 )
		tcMod scale -1 -1
		tcMod scroll 0 -0.25
		depthFunc equal
		detail
	}
	{
		map ui/assets5/buttonmask.tga
		blendfunc add
		rgbGen const ( 0.913726 0.552941 0.196078 )
	}
}

azurebutton
{
	nopicmip
	{
		map ui/assets5/buttonmask.tga
		blendfunc add
		depthWrite
		alphaFunc GE128
		detail
	}
	{
		map ui/assets5/techyscroll3.tga
		blendfunc blend
		rgbGen Vertex
		tcMod scroll 0 0.2
		depthFunc equal
	}
	{
		map ui/assets5/techyscroll3.tga
		blendfunc blend
		rgbGen Vertex
		tcMod scale -1 -1
		tcMod scroll 0 -0.25
		depthFunc equal
		detail
	}
	{
		map ui/assets5/buttonmask.tga
		blendfunc add
		rgbGen Vertex
	}
}

azuretablongforced
{
	nomipmaps
	{
		map ui/assets5/tablong.tga
		blendfunc blend
		rgbGen const ( 0.156863 0.188235 0.258824 )
	}
	{
		map ui/assets5/buttonlong2.tga
		blendfunc add
		rgbGen Vertex
	}
}

azuretablong
{
	nomipmaps
	{
		map ui/assets5/tablong.tga
		blendfunc blend
		rgbGen Vertex
	}
	{
		map ui/assets5/buttonlong2.tga
		blendfunc add
		rgbGen Vertex
	}
}

errorscroll
{
	{
		map ui/assets5/errorscroll.tga
		blendfunc blend
		rgbGen const ( 0.380392 0.0392157 0.0392157 )
		tcMod scale 1 4
		tcMod scroll 0 -1
		alphaGen wave sin 0.7 0.2 0 1 
	}
}

debugbg1
{
	{
		map $whiteimage 
		rgbGen const ( 0.45098 0.490196 0.317647 )
	}
}

OALOGOMODEL
{
	{
		map ui/assetspcx.tga
		tcGen environment 
	}
}



models/ui/oalogo128
{


	{
		map models/ui/oalogo128ref.tga
		tcGen environment 
	}
	{
		map models/ui/oalogo128.tga
		blendFunc blend
	}


}


debugbg2
{
	{
		map $whiteimage 
		blendfunc blend
		rgbGen const ( 0.301961 0.45098 0.509804 )
		alphaGen wave sin 0.2 0 0 0 
	}
}

gamestartingfade
{
	{
		map ui/assets5/fadedown.tga
		blendfunc filter
		rgbGen Vertex
	}
}

azureuntablongforced
{
	nomipmaps
	{
		map ui/assets5/tablong.tga
		blendfunc blend
		rgbGen const ( 0.156863 0.188235 0.258824 )
	}
}

trophy_gold
{
	nomipmaps
	{
		map gfx/fx/flares/largeglow.tga
		blendfunc add
		rgbGen Vertex
	}
}

azureuntablong
{
	nomipmaps
	{
		map ui/assets5/tablong.tga
		blendfunc blend
		rgbGen Vertex
	}
}

azurefadein
{
	{
		map ui/assets5/fadein.tga
		blendfunc blend
		tcMod stretch sawtooth 1 2 0 1 
		alphaGen Vertex
	}
}

azurebuttonlongforced
{
	nomipmaps
	{
		map ui/assets5/buttonlong1.tga
		blendfunc blend
		rgbGen const ( 0.156863 0.188235 0.258824 )
	}
	{
		map ui/assets5/buttonlong2.tga
		blendfunc add
		rgbGen Vertex
	}
}

azurebuttonlong
{
	nomipmaps
	{
		map ui/assets5/buttonlong1.tga
		blendfunc blend
		rgbGen Vertex
	}
}

azurebuttonlonghi
{
	nomipmaps
	{
		map ui/assets5/buttonlong1.tga
		blendfunc blend
		rgbGen const ( 0.156863 0.188235 0.258824 )
	}
	{
		map ui/assets5/discplayer.tga
		blendfunc add
		rgbGen const ( 0.435294 0.776471 0.223529 )
		tcMod stretch sin 1.3 0.3 0 1 
	}
}

azurebuttoncomboforced
{
	{
		map ui/assets5/comboboxheader.tga
		blendfunc blend
		rgbGen const ( 0.156863 0.188235 0.258824 )
	}
}

azurebuttoncombo
{
	{
		map ui/assets5/comboboxheader.tga
		blendfunc blend
		rgbGen Vertex
	}
}

azurebuttonreallylong
{
	nomipmaps
	{
		map ui/assetspcx/longgrad.tga
		blendfunc add
		rgbGen const ( 0.054902 0.0627451 0.0784314 )
	}
}

azurebuttonreallylonga
{
	nomipmaps
	{
		map ui/assetspcx/longgrad.tga
		blendfunc add
		rgbGen const ( 0.113725 0.14902 0.305882 )
	}
}

azurebuttonreallylongaOLD
{
	nomipmaps
	{
		map ui/assets5/longgrad.tga
		blendfunc add
		rgbGen const ( 0.113725 0.14902 0.305882 )
	}
	{
		map ui/assets5/discplayer.tga
		blendfunc add
		rgbGen const ( 0.172549 0.176471 0.227451 )
		tcMod stretch sawtooth 1 1 0 32 
		tcMod rotate 256
	}
}

azuredisc
{
	{
		map ui/assets5/discplayer.tga
		blendfunc add
		rgbGen const ( 0.321569 0.537255 0.552941 )
		tcMod stretch sin 1.5 0.2 0 1 
		tcMod rotate 64
	}
	{
		map ui/assets5/discplayer.tga
		blendfunc add
		rgbGen const ( 0.313726 0.176471 0.341176 )
		tcMod rotate -64
	}
}

dialogbg_azureforced
{
	nomipmaps
	{
		map $whiteimage 
		blendfunc blend
		rgbGen const ( 0.0823529 0.101961 0.12549 )
		alphaGen wave square 0.7 0 0 0 
	}
}

dialogbg_azure
{
	nomipmaps
	deformVertexes normal 7 0.5
	{
		map ui/assets5/uishade.tga
		blendfunc blend
		rgbGen Vertex
	}
	{
		map ui/assets5/scan.tga
		blendfunc gl_dst_color gl_src_color
		rgbGen Identity
		tcGen vector ( 0.03125 0 0 ) ( 0 0.03125 0 ) 
		
	}

}

dialogbg_azurej
{
	nomipmaps
	{
		map $whiteimage 
		blendfunc blend
		rgbGen Vertex
		alphaGen Vertex
	}
}

dialogbg_azureALPHA
{
	nomipmaps
	{
		map ui/assets5/techyscroll1c.tga
		blendfunc blend
		rgbGen const ( 0.117647 0.137255 0.215686 )
		tcMod scale 0.25 0.25
		tcMod scroll 0.01 -0.01
		alphaGen wave square 0.7 0 0 0 
	}
	{
		map ui/assets5/techyscroll1.tga
		blendfunc blend
		rgbGen const ( 0.113725 0.160784 0.172549 )
		tcMod scale -0.25 -0.25
		tcMod scroll 0.01 0.01
		alphaGen wave square 0.9 0 0 0 
		detail
	}
}

listbg_azureforced
{
	nomipmaps
	{
		map ui/assetspcx/techyscroll1c.tga
		rgbGen const ( 0.027451 0.027451 0.0392157 )
		tcMod scale 0.25 0.25
		tcMod scroll 0.01 -0.01
		alphaGen wave square 0.9 0 0 0 
	}
}

listbg_azure
{
	nomipmaps
	{
		map ui/assetspcx/techyscroll1c.tga
		rgbGen Vertex
		tcMod scale 0.25 0.25
		tcMod scroll 0.01 -0.01
		alphaGen wave square 0.9 0 0 0 
	}
}

listbg_azureALPHA
{
	nomipmaps
	{
		map ui/assetspcx/techyscroll1c.tga
		blendfunc blend
		rgbGen const ( 0.0784314 0.0823529 0.0941177 )
		tcMod scale 0.25 0.25
		tcMod scroll 0.01 -0.01
		alphaGen wave square 0.9 0 0 0 
	}
	{
		map ui/assetspcx/techyscroll1.tga
		blendfunc blend
		rgbGen const ( 0.141176 0.113725 0.160784 )
		tcMod scale -0.25 -0.25
		tcMod scroll 0.01 0.01
		alphaGen wave square 0.9 0 0 0 
		detail
	}
}

AzureBG
{
	nomipmaps
	deformVertexes wave 32 sin 0 57 0 0.25 
	deformVertexes wave 344 sin 33 95 9 0.04 

	{
		map ui/assets5/cldbgb.tga
		blendfunc add
		tcMod turb 0 0.05 3 0.1
		tcGen environment 

	}
}


AzureBGBlendover
{
	nomipmaps
	{
		map ui/assets5/cldbgb.tga
		blendfunc gl_dst_color gl_one
		rgbGen vertex
		tcMod rotate 2
		tcMod scale 4 1
		tcMod scroll 0.2 0.2
		tcMod turb 0 0.4 3 0.1


	}
}



scary1
{

	nomipmaps
	{
		map ui/assets5/flight1.tga
		blendfunc gl_dst_color gl_one
		rgbGen vertex
	}
	{
		map ui/assets5/flight1.tga
		blendfunc gl_dst_color gl_one
		rgbGen vertex
	}

	{
		map ui/assets5/flight1.tga
		blendfunc gl_dst_color gl_one
		rgbGen vertex
	}

	{
		map ui/assets5/flight1.tga
		blendfunc gl_dst_color gl_one
		rgbGen vertex
	}

}


scary2
{
	nomipmaps
	{
		map ui/assets5/vignette.tga
		blendfunc gl_dst_color gl_src_color
		rgbGen vertex
	}
}

background_azureOLD
{
	nomipmaps
	{
		map ui/assets5/techyscroll1.tga
		rgbGen const ( 0.309804 0.207843 0.4 )
		tcMod scale 0.25 0.25
		tcMod scroll 0.01 -0.01
	}
	{
		map ui/assets5/techyscroll1.tga
		blendfunc blend
		rgbGen const ( 0.384314 0.72549 0.784314 )
		tcMod scale -0.25 -0.25
		tcMod scroll -0.01 0.01
		detail
	}
	{
		map ui/assets5/techyscroll2.tga
		blendfunc add
		rgbGen const ( 0.537255 0.627451 0.796079 )
		tcMod scale 1 0.1
		tcMod scroll 0.5 0.1
	}
	{
		map ui/assets5/hexapattern1.tga
		blendfunc blend
		rgbGen const ( 0 0 0 )
		tcMod turb 0 0.05 0 0.1
		tcMod scale 2 1.5
		tcMod scroll 0.25 0.25
	}
}

background_azure
{
	{
		map $whiteimage 
		rgbGen const ( 0 0 0 )
	}
}

multdown
{
	{
		map $whiteimage 
		blendfunc filter
		rgbGen Vertex
	}
}

dialogbg
{
	{
		map ui/assets4/boxscroll.tga
		rgbGen Vertex
		tcMod scroll 0.125 0.125
		tcMod scale 2 2
	}
	{
		map ui/assets4/boxbg.tga
		blendfunc gl_dst_color gl_src_color
		rgbGen identity
	}
}

mpbgvideo
{
	{
		map ui/assets3/bg/slimefac.tga
		rgbGen const ( 0.388235 0.729412 0.745098 )
	}
}

mpingamebg
{
	{
		map $whiteimage 
		blendfunc gl_dst_color gl_src_color
		rgbGen const ( 0 0 1 )
	}
	{
		map $whiteimage 
		blendfunc blend
		alphaGen wave sin 0.3 0 0 1 
	}
	{
		map ui/assets3/ingamebg.tga
		blendfunc filter
		alphaGen wave sin 0.5 0 0 1 
	}
}

mpbgerror
{
	{
		map $whiteimage 
		blendfunc filter
		rgbGen const ( 0.584314 0 0 )
	}
}

mpingamemenu
{
	{
		map ui/assets3/ingamemenu.tga
		rgbGen identityLighting
		alphaFunc GE128
	}
}

mpmainbutton
{
	{
		map ui/assets3/longbtnl.tga
		blendfunc filter
		rgbGen identityLighting
		alphaFunc GE128
	}
}

mpfoptionsbg
{
	{
		map ui/assets3/scrollbg.tga
		rgbGen const ( 0.219608 0.262745 0.286275 )
		tcMod scroll 1 1
		alphaFunc GE128
	}
}

mpconnectbg
{
	{
		map ui/assets3/bg/slimefac.tga
		rgbGen identity
	}
}

mpblankbg
{
	{
		map $whiteimage 
		rgbGen identityLighting
	}
}

menuarc
{
	{
		map ui/assets3/mainarc.tga
		rgbGen identityLighting
		alphaFunc GE128
	}
}

menuarc2
{
	{
		map $whiteimage 
		blendfunc filter
		rgbGen const ( 0.258824 0.670588 0.741176 )
	}
	{
		map ui/assets3/mainarc.tga
		rgbGen identityLighting
		alphaFunc GE128
	}
}

mpoptionsbg
{
	nopicmip
	{
		map ui/assets3/optbg.tga
		blendfunc filter
		rgbGen const ( 0.247059 0.447059 0.403922 )
		alphaFunc GE128
	}
}

mpcharsconnect
{
	{
		animmap 0.1 ui/assets3/poses/dark.tga ui/assets3/poses/dark1sa.tga ui/assets3/poses/dark1s.tga ui/assets3/poses/dark1.tga 
		blendfunc add
		rgbGen identityLighting
		tcMod scale 1 1
	}
}

mpscrollbg
{
	{
		map ui/assets3/scrollbg.tga
		blendfunc gl_dst_color gl_src_color
		rgbGen identityLighting
		tcMod scroll 0.1 0.2
		tcMod turb 0 0.1 0 0.18
		detail
	}
	{
		map ui/assets3/scrollbg.tga
		blendfunc gl_dst_color gl_src_color
		rgbGen identityLighting
		tcMod scroll -0.1 -0.2
		tcMod turb 0 0.1 0 0.1
		detail
	}
}

lowhud_left
{
	nopicmip
	{
		map ui/assets3/lowhudlft.tga
		blendfunc add
		alphaFunc GE128
	}
}

lowhud_right
{
	nopicmip
	{
		map ui/assets3/lowhudrt.tga
		blendfunc add
		alphaFunc GE128
	}
}



// used for console menu which overlays over.
ingamemenublendbg
{

	{
		map $whiteimage
		
		blendfunc GL_DST_COLOR GL_SRC_COLOR
		rgbGen Vertex
	}
}





