#include <SDL.h>
#include <SDL_ttf.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bodies.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#define PI 3.14159265358979323846
#define MAX_STARS 720
#define SURFACE_SIZE 160
#define GOLD (SDL_Color){201,169,110,255}
#define INK (SDL_Color){232,226,208,255}
#define MUTED (SDL_Color){138,133,120,255}
#define FAINT (SDL_Color){91,86,75,255}

typedef enum { VIEW_PLANETS, VIEW_DWARFS, VIEW_MILKYWAY, VIEW_GALAXIES } View;
typedef struct { float x,y,r,phase; SDL_Color color; } Star;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *surfaces[BODY_COUNT];
    TTF_Font *serif, *serif_italic, *serif_title_medium, *serif_title_small, *serif_title_tiny;
    TTF_Font *serif_small_italic, *mono, *mono_small;
    int w,h;
    bool running, entered, info_open, dragging;
    bool smoke;
    View view;
    int selected;
    float camera_rotation, tilt, zoom, time, scroll;
    int mouse_x, mouse_y, drag_x, drag_y;
    Star stars[MAX_STARS];
} App;

static App app;
#ifdef __EMSCRIPTEN__
static const char *FONT_SERIF = "/fonts/DejaVuSerif.ttf";
static const char *FONT_SERIF_ITALIC = "/fonts/DejaVuSerif.ttf";
static const char *FONT_ITALIC = "/fonts/LiberationSerif-Italic.ttf";
static const char *FONT_MONO = "/fonts/DejaVuSansMono.ttf";
#else
static const char *FONT_SERIF = "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf";
static const char *FONT_SERIF_ITALIC = "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf";
static const char *FONT_ITALIC = "/usr/share/fonts/truetype/liberation/LiberationSerif-Italic.ttf";
static const char *FONT_MONO = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf";
#endif

static SDL_Color rgb(Uint32 c, Uint8 a) { return (SDL_Color){(c>>16)&255,(c>>8)&255,c&255,a}; }
static int clampi(int x,int a,int b){ return x<a?a:(x>b?b:x); }
static float clampf(float x,float a,float b){ return x<a?a:(x>b?b:x); }
static void color(SDL_Color c){ SDL_SetRenderDrawColor(app.renderer,c.r,c.g,c.b,c.a); }
static void fill_rect(int x,int y,int w,int h,SDL_Color c){ color(c); SDL_Rect r={x,y,w,h}; SDL_RenderFillRect(app.renderer,&r); }
static void line(int x1,int y1,int x2,int y2,SDL_Color c){ color(c); SDL_RenderDrawLine(app.renderer,x1,y1,x2,y2); }
static void circle_outline(int x,int y,int r,SDL_Color c){ if(r<1)return; color(c); for(int i=0;i<96;i++){float a=i*2*PI/96,b=(i+1)*2*PI/96; SDL_RenderDrawLine(app.renderer,x+cosf(a)*r,y+sinf(a)*r,x+cosf(b)*r,y+sinf(b)*r);} }
static void filled_circle(int x,int y,int r,SDL_Color c){ if(r<1)return; color(c); for(int dy=-r;dy<=r;dy++){int dx=(int)sqrtf((float)r*r-(float)dy*dy); SDL_RenderDrawLine(app.renderer,x-dx,y+dy,x+dx,y+dy);} }
static Uint32 noise_hash(int x,int y,int seed){Uint32 n=(Uint32)x*0x1f123bb5u^(Uint32)y*0x5f356495u^(Uint32)seed*0x6c8e9cf5u;n^=n>>16;n*=0x7feb352du;n^=n>>15;n*=0x846ca68bu;n^=n>>16;return n;}
static float noise_lattice(int x,int y,int seed){return (float)(noise_hash(x,y,seed)&0x00ffffffu)/8388607.5f-1.0f;}
static float value_noise(float x,float y,int seed){int ix=(int)floorf(x),iy=(int)floorf(y);float fx=x-ix,fy=y-iy;fx=fx*fx*(3-2*fx);fy=fy*fy*(3-2*fy);float a=noise_lattice(ix,iy,seed),b=noise_lattice(ix+1,iy,seed),c=noise_lattice(ix,iy+1,seed),d=noise_lattice(ix+1,iy+1,seed);float ab=a+(b-a)*fx,cd=c+(d-c)*fx;return ab+(cd-ab)*fy;}
static float fractal_noise(float x,float y,int seed){float value=0,amp=.56f,norm=0;for(int octave=0;octave<4;octave++){value+=value_noise(x,y,seed+octave*71)*amp;norm+=amp;x*=2.03f;y*=2.03f;amp*=.48f;}return value/norm;}
static bool name_is(const Body *b,const char *name){return strcmp(b->name,name)==0;}
static void surface_palette(const Body *b,int *r,int *g,int *bl){*r=(b->color>>16)&255;*g=(b->color>>8)&255;*bl=b->color&255;if(name_is(b,"Mercury")){*r=139;*g=127;*bl=111;}else if(name_is(b,"Venus")){*r=206;*g=151;*bl=79;}else if(name_is(b,"Earth")){*r=37;*g=105;*bl=175;}else if(name_is(b,"Mars")){*r=180;*g=73;*bl=39;}else if(name_is(b,"Jupiter")){*r=179;*g=119;*bl=73;}else if(name_is(b,"Saturn")){*r=203;*g=174;*bl=120;}else if(name_is(b,"Uranus")){*r=105;*g=189;*bl=201;}else if(name_is(b,"Neptune")){*r=40;*g=79;*bl=193;}else if(name_is(b,"Pluto")){*r=178;*g=134;*bl=99;}else if(name_is(b,"Eris")){*r=191;*g=193;*bl=199;}else if(name_is(b,"Haumea")){*r=180;*g=159;*bl=135;}else if(name_is(b,"Makemake")){*r=159;*g=91;*bl=67;}else if(name_is(b,"Ceres")){*r=121;*g=119;*bl=112;}}
static bool is_rocky(const Body *b){return name_is(b,"Mercury")||name_is(b,"Mars")||name_is(b,"Ceres")||name_is(b,"Pluto")||name_is(b,"Eris")||name_is(b,"Haumea")||name_is(b,"Makemake");}
static SDL_Texture *make_planet_surface(const Body *b,int index){SDL_Surface *surface=SDL_CreateRGBSurfaceWithFormat(0,SURFACE_SIZE,SURFACE_SIZE,32,SDL_PIXELFORMAT_ARGB8888);if(!surface)return NULL;int br,bg,bb;surface_palette(b,&br,&bg,&bb);bool earth=name_is(b,"Earth"),venus=name_is(b,"Venus"),jupiter=name_is(b,"Jupiter"),saturn=name_is(b,"Saturn"),ice_giant=name_is(b,"Uranus")||name_is(b,"Neptune");Uint32 *pixels=(Uint32 *)surface->pixels;int pitch=surface->pitch/4;float light_x=-.38f,light_y=-.44f,light_z=.81f;
    for(int py=0;py<SURFACE_SIZE;py++)for(int px=0;px<SURFACE_SIZE;px++){float nx=(2.0f*(px+.5f)/SURFACE_SIZE)-1.0f,ny=(2.0f*(py+.5f)/SURFACE_SIZE)-1.0f,rr=nx*nx+ny*ny;int r=br,g=bg,bl=bb,a=0;if(rr<=.98f){float nz=sqrtf(fmaxf(0,1-rr));a=255;float rough=fractal_noise(nx*5.0f,ny*5.0f,index*137+19),fine=value_noise(nx*19,ny*19,index*311+3),illum=fmaxf(.16f,nx*light_x+ny*light_y+nz*light_z);illum=.30f+.78f*illum;float shade=1.0f+rough*.20f+fine*.075f;
            if(earth){float land=fractal_noise(nx*3.7f+1.4f,ny*3.7f-2.1f,index*137+19)+value_noise(nx*8,ny*8,index*137+91)*.16f;if(land>.10f){r=91+(int)(rough*19);g=127+(int)(rough*34);bl=65+(int)(rough*22);}else{r=17+(int)(rough*15);g=71+(int)(rough*30);bl=132+(int)(rough*48);}float cloud=fractal_noise(nx*13,ny*13,index*137+401);if(cloud>.47f){r=(r+225)/2;g=(g+235)/2;bl=(bl+241)/2;}}
            else if(jupiter||saturn||ice_giant){float band=ny+(rough+fine*.35f)*.055f;float stripe=sinf(band*(jupiter?43.0f:saturn?35.0f:28.0f)+(jupiter?1.0f:0.0f));float contrast=jupiter?.22f:saturn?.16f:.12f;shade*=1.0f+stripe*contrast;if(jupiter){float storm=sqrtf((nx-.32f)*(nx-.32f)*2.2f+(ny+.20f)*(ny+.20f)*5.0f);if(storm<.30f){r=190;g=83+(int)(storm*80);bl=53;shade*=1.12f;}}if(ice_giant&&rough>.46f){r+=20;g+=22;bl+=23;}}
            else if(venus){float swirl=sinf((ny+rough*.11f)*17+nx*3.1f);shade*=.94f+swirl*.13f;if(fabsf(rough)>.48f){r+=20;g+=11;bl-=3;}}
            else if(is_rocky(b)){shade*=1.0f+rough*.18f;for(int crater=0;crater<15;crater++){float cx=noise_lattice(crater*17+index,crater*31+9,index+33)*.72f,cy=noise_lattice(crater*23+5,crater*13+index,index+79)*.72f,rad=.035f+((noise_hash(crater+index*7,crater*3+11,index)&255)/255.0f)*.095f,dx=nx-cx,dy=ny-cy,dist=sqrtf(dx*dx+dy*dy);if(dist<rad){float rim=1.0f-fabsf(dist-rad*.83f)/(rad*.24f);shade*=dist<rad*.65f?.80f:1.02f;if(rim>0.0f)shade+=rim*.16f;}}if(name_is(b,"Mars")&&ny<-.55f)shade*=1.10f;}
            float fresnel=(1-nz)*(1-nz);if(earth){r=(int)(r*shade*illum);g=(int)(g*shade*illum);bl=(int)(bl*shade*illum);r+=(int)(fresnel*19);g+=(int)(fresnel*40);bl+=(int)(fresnel*68);}else{r=(int)(r*shade*illum);g=(int)(g*shade*illum);bl=(int)(bl*shade*illum);if(ice_giant){r+=(int)(fresnel*15);g+=(int)(fresnel*39);bl+=(int)(fresnel*54);}}
            r=clampi(r,0,255);g=clampi(g,0,255);bl=clampi(bl,0,255);}
        pixels[py*pitch+px]=SDL_MapRGBA(surface->format,(Uint8)r,(Uint8)g,(Uint8)bl,(Uint8)a);}
    SDL_Texture *texture=SDL_CreateTextureFromSurface(app.renderer,surface);SDL_FreeSurface(surface);if(texture)SDL_SetTextureBlendMode(texture,SDL_BLENDMODE_BLEND);return texture;}
static void ellipse_outline(int x,int y,int rx,int ry,SDL_Color c){ if(rx<1||ry<1)return; color(c); for(int i=0;i<120;i++){float a=i*2*PI/120,b=(i+1)*2*PI/120; SDL_RenderDrawLine(app.renderer,x+cosf(a)*rx,y+sinf(a)*ry,x+cosf(b)*rx,y+sinf(b)*ry);} }
static void text_raw(const char *s,int x,int y,TTF_Font *font,SDL_Color c){ if(!s||!*s)return; SDL_Surface *surf=TTF_RenderUTF8_Blended(font,s,c); if(!surf)return; SDL_Texture *t=SDL_CreateTextureFromSurface(app.renderer,surf); if(t){SDL_Rect r={x,y,surf->w,surf->h}; SDL_RenderCopy(app.renderer,t,NULL,&r); SDL_DestroyTexture(t);} SDL_FreeSurface(surf); }
static int text_width(const char *s,TTF_Font *f){int w=0,h=0; if(s&&*s)TTF_SizeUTF8(f,s,&w,&h); return w;}
static void spaced_text(const char *s,int x,int y,TTF_Font *font,SDL_Color c,int gap){
    const unsigned char *p=(const unsigned char *)s;
    while(*p){int n=(*p<0x80)?1:((*p&0xe0)==0xc0)?2:((*p&0xf0)==0xe0)?3:4;char ch[5]={0};for(int i=0;i<n&&p[i];i++)ch[i]=(char)p[i];text_raw(ch,x,y,font,c);x+=text_width(ch,font)+gap;p+=n;}
}
static bool inside(int x,int y,int rx,int ry,int rw,int rh){return x>=rx&&x<=rx+rw&&y>=ry&&y<=ry+rh;}
static int panel_width(void){if(!app.info_open)return 0;return app.w<800?(int)fmaxf(230.0f,app.w*.72f):420;}
static int wrapped_text(const char *s,int x,int y,int maxw,TTF_Font *f,SDL_Color c,int leading){
    if(!s)return y;
    char word[512],linebuf[1200]=""; const char *p=s; int len=0;
    while(*p){while(*p==' '||*p=='\n'||*p=='\t')p++; if(!*p)break; const char *e=p; while(*e&&*e!=' '&&*e!='\n'&&*e!='\t')e++; len=(int)(e-p); if(len>500)len=500; memcpy(word,p,len); word[len]=0;
        char test[1200]; if(linebuf[0])snprintf(test,sizeof(test),"%s %s",linebuf,word); else snprintf(test,sizeof(test),"%s",word);
        if(linebuf[0]&&text_width(test,f)>maxw){text_raw(linebuf,x,y,f,c); y+=leading; snprintf(linebuf,sizeof(linebuf),"%s",word);} else snprintf(linebuf,sizeof(linebuf),"%s",test);
        p=e;
    }
    if(linebuf[0]){text_raw(linebuf,x,y,f,c); y+=leading;} return y;
}
static int count_view(void){if(app.view==VIEW_PLANETS)return 8;if(app.view==VIEW_DWARFS)return 5;if(app.view==VIEW_MILKYWAY)return 1;return 6;}
static int view_index(int n){if(app.view==VIEW_PLANETS)return n;if(app.view==VIEW_DWARFS)return 8+n;if(app.view==VIEW_MILKYWAY)return 13;return 14+n;}
static int selected_slot(void){for(int i=0;i<count_view();i++)if(view_index(i)==app.selected)return i;return 0;}
static void change_view(View v){app.view=v;app.selected=view_index(0);app.info_open=true;app.scroll=0;app.zoom=1;app.camera_rotation=0;app.tilt=0.30f;}
static void step_selection(int d){int n=count_view(),slot=(selected_slot()+d+n)%n;app.selected=view_index(slot);app.info_open=true;app.scroll=0;}
static void stars_init(void){srand(2026);for(int i=0;i<MAX_STARS;i++){Star *s=&app.stars[i];s->x=(float)rand()/(float)RAND_MAX;s->y=(float)rand()/(float)RAND_MAX;s->r=(float)(rand()%16+4)/10.0f;s->phase=(float)rand()/(float)RAND_MAX*6.28f;int t=rand()%3;s->color=t==0?(SDL_Color){255,226,180,255}:t==1?(SDL_Color){190,210,255,255}:(SDL_Color){232,226,208,255};}}
static void draw_backdrop(void){fill_rect(0,0,app.w,app.h,(SDL_Color){3,5,11,255});
    for(int i=0;i<MAX_STARS;i++){Star *s=&app.stars[i];int x=(int)(s->x*app.w),y=(int)(s->y*app.h);float tw=.65f+.35f*sinf(app.time*1.4f+s->phase);SDL_Color c=s->color;c.a=(Uint8)(170*tw);filled_circle(x,y,(int)fmaxf(1,s->r),c);}
    // A restrained, low-contrast nebula haze.
    for(int k=0;k<7;k++){int x=app.w*(.15f+k*.13f),y=app.h*(.28f+(k%3)*.19f);SDL_Color c=k%2?(SDL_Color){28,32,61,19}:(SDL_Color){45,28,55,17};filled_circle(x,y,90+(k%3)*35,c);}
}
static void draw_sun(int x,int y,float scale){int r=(int)(10*scale);if(r<3)r=3;filled_circle(x,y,r+15,(SDL_Color){85,41,14,32});filled_circle(x,y,r+8,(SDL_Color){190,91,29,65});filled_circle(x,y,r,(SDL_Color){255,160,59,255});filled_circle(x,y,r-3,(SDL_Color){255,218,130,255});filled_circle(x,y,r-6,(SDL_Color){255,244,207,255});}
static void draw_planet_body(const Body *b,int index,int x,int y,int r){int texture_size=r*2+2;SDL_Rect dst={x-texture_size/2,y-texture_size/2,texture_size,texture_size};if(b->rings){ellipse_outline(x,y,r*2+14,(int)(r*.58f)+6,(SDL_Color){112,101,83,160});ellipse_outline(x,y,r*2+9,(int)(r*.52f)+3,(SDL_Color){235,213,166,190});}SDL_Texture *texture=app.surfaces[index];if(texture)SDL_RenderCopy(app.renderer,texture,NULL,&dst);else filled_circle(x,y,r,rgb(b->color,255));if(name_is(b,"Earth")){filled_circle(x-r/3,y-r/3,(int)fmaxf(1,r*.10f),(SDL_Color){206,233,255,95});circle_outline(x,y,r+2,(SDL_Color){124,203,255,190});}else if(name_is(b,"Venus")||name_is(b,"Jupiter")||name_is(b,"Saturn")){circle_outline(x,y,r+2,(SDL_Color){255,216,151,130});}else if(name_is(b,"Uranus")||name_is(b,"Neptune")){circle_outline(x,y,r+2,(SDL_Color){107,190,255,165});}else circle_outline(x,y,r+1,(SDL_Color){222,205,174,110});if(b->rings){ellipse_outline(x,y,r*2+14,(int)(r*.58f)+6,(SDL_Color){242,224,181,220});}}
static int planet_screen(const Body *b,float scale,int cx,int cy,float *ox,float *oy){float a=app.time*b->orbit_speed*42.0f+app.camera_rotation;float d=b->distance;*ox=cx+cosf(a)*d*scale;*oy=cy+sinf(a)*d*scale*app.tilt;return (int)fmaxf(3,4.4f+sqrtf(fmaxf(.03f,b->radius))*7.0f);}
static void draw_orbit_scene(void){int panel=panel_width();int cx=(app.w-panel)/2,cy=app.h/2;float maxd=app.view==VIEW_PLANETS?102.0f:130.0f;float scale=fminf((app.w-panel)*.43f/maxd,app.h*.36f/maxd)*app.zoom;
    if(app.view==VIEW_PLANETS||app.view==VIEW_DWARFS)draw_sun(cx,cy,app.zoom);
    if(app.view==VIEW_PLANETS||app.view==VIEW_DWARFS){
        if(app.view==VIEW_PLANETS){for(int j=0;j<8;j++){float d=bodies[j].distance;ellipse_outline(cx,cy,(int)(d*scale),(int)(d*scale*app.tilt),(SDL_Color){55,78,112,66});}
            // The asteroid belt between Mars and Jupiter.
            for(int i=0;i<430;i++){float a=(float)i*2.39996f;float d=42.0f+fmodf(i*17.31f,6.0f);int x=cx+(int)(cosf(a+app.camera_rotation)*d*scale),y=cy+(int)(sinf(a+app.camera_rotation)*d*scale*app.tilt);filled_circle(x,y,(i%11==0)?2:1,(SDL_Color){125,121,112,120});}}
        int first=app.view==VIEW_PLANETS?0:8,last=app.view==VIEW_PLANETS?8:13;
        for(int i=first;i<last;i++){const Body *b=&bodies[i];float fx,fy;int r=planet_screen(b,scale,cx,cy,&fx,&fy);int x=(int)fx,y=(int)fy; if(b->moon_count){for(int m=0;m<b->moon_count;m++){float a=app.time*b->moons[m].speed*26+i*.8f;int mx=x+(int)(cosf(a)*(r+5+m*3)),my=y+(int)(sinf(a)*(r+5+m*3)*.58f);filled_circle(mx,my,clampi((int)(b->moons[m].radius*6),1,3),rgb(b->moons[m].color,220));}}
            draw_planet_body(b,i,x,y,r);if(i==app.selected){circle_outline(x,y,r+7,(SDL_Color){201,169,110,170});}
        }
    }else if(app.view==VIEW_MILKYWAY){
        int gx=cx,gy=cy;float sz=fminf(app.w*.28f,app.h*.36f)*app.zoom;
        for(int arm=0;arm<4;arm++){float base=arm*PI/2;for(int j=0;j<340;j++){float t=(float)j/340,rad=sz*(.08f+t*.91f),a=base+t*PI*2.5f+app.time*.025f;float jitter=sinf(j*12.73f+arm)*sz*.018f;int x=gx+cosf(a)*rad+(int)jitter,y=gy+sinf(a)*rad*app.tilt+(int)(cosf(j*4.13f)*jitter);SDL_Color c=j%9==0?(SDL_Color){255,222,173,220}:(SDL_Color){177,190,220,(Uint8)(60+120*(1-t))};filled_circle(x,y,j%17==0?2:1,c);}}
        filled_circle(gx,gy,(int)(sz*.1f),(SDL_Color){239,197,130,60});filled_circle(gx,gy,(int)(sz*.05f),(SDL_Color){255,231,185,200});
        int sx=gx+(int)(sz*.47f),sy=gy-(int)(sz*.13f*app.tilt);filled_circle(sx,sy,5,(SDL_Color){255,216,102,255});circle_outline(sx,sy,11,(SDL_Color){255,216,102,110});text_raw("SOL · ORION ARM",sx+15,sy-8,app.mono_small,GOLD);
    }else{
        static const float pos[6][2]={{-.65f,-.25f},{.15f,-.35f},{.68f,-.13f},{-.48f,.32f},{.45f,.35f},{-.08f,.12f}};
        for(int n=0;n<6;n++){int i=14+n;const Body *b=&bodies[i];int x=cx+(int)(pos[n][0]*app.w*.40f),y=cy+(int)(pos[n][1]*app.h*.52f);float sz=clampf(13+b->galaxy_size*.20f,15,48);if(i==app.selected)sz*=1.25f;
            if(!strcmp(b->galaxy_type,"irregular")){for(int j=0;j<160;j++){float a=j*2.399f,rad=sz*sqrtf((float)j/160);filled_circle(x+cosf(a)*rad,y+sinf(a)*rad*.65f,1,rgb(b->color,(Uint8)(80+(j*37)%100)));}}
            else {int arms=strcmp(b->galaxy_type,"edgeon")==0?1:2;for(int a=0;a<arms;a++)for(int j=0;j<100;j++){float t=(float)j/100,r0=sz*(.12f+t*.84f),ang=a*PI+t*PI*3.3f;filled_circle(x+cosf(ang)*r0,y+sinf(ang)*r0*(strcmp(b->galaxy_type,"edgeon")==0?.22f:.64f),j%9==0?2:1,rgb(b->color,(Uint8)(85+135*(1-t))));}}
            filled_circle(x,y,(int)(sz*.12f),(SDL_Color){255,239,206,170});if(i==app.selected)circle_outline(x,y,(int)sz+8,GOLD);text_raw(b->name,x-(text_width(b->name,app.mono_small)/2),y+(int)sz+10,app.mono_small,i==app.selected?GOLD:MUTED);
        }
    }
}
static void draw_button(int x,int y,int w,int h,const char *label,bool active){SDL_Color bg=active?(SDL_Color){49,40,27,235}:(SDL_Color){12,14,21,218};fill_rect(x,y,w,h,bg);SDL_Color border=active?(SDL_Color){201,169,110,160}:(SDL_Color){92,84,67,120};line(x,y,x+w,y,border);line(x,y+h,x+w,y+h,border);line(x,y,x,y+h,border);line(x+w,y,x+w,y+h,border);int tw=text_width(label,app.mono_small);text_raw(label,x+(w-tw)/2,y+(h-13)/2,app.mono_small,active?GOLD:MUTED);}
static int view_button_width(int i){return app.w<520?(i==2?88:67):(i==2?118:96);}
static int masthead_left(void){return app.w<360?12:app.w<520?18:34;}
static void draw_masthead(void){int left_x=masthead_left();TTF_Font *heading=app.w<520?app.serif_title_tiny:app.serif_small_italic;spaced_text("VOL. I  ·  TRANSMISSION 28.08.2026",left_x,27,app.mono_small,MUTED,1);const char *left="Field Notes from the";text_raw(left,left_x,47,heading,INK);text_raw("Outer Dark",left_x+text_width(left,heading)+7,47,heading,GOLD);
    const char *labels[]={"PLANETS","DWARFS","MILKY WAY","GALAXIES"};int x=left_x,y=88;for(int i=0;i<4;i++){int ww=view_button_width(i);draw_button(x,y,ww,30,labels[i],app.view==(View)i);x+=ww+5;}
}
static void draw_data_panel(void){if(!app.info_open)return;int pw=panel_width(),x=app.w-pw,pad=pw<350?24:38;fill_rect(x,0,pw,app.h,(SDL_Color){5,7,12,238});fill_rect(x,0,1,app.h,(SDL_Color){201,169,110,100});int inner=x+pad,top=46;SDL_Rect clip={x+14,24,pw-28,app.h-48};SDL_RenderSetClipRect(app.renderer,&clip);int y=top-(int)app.scroll;const Body *b=&bodies[app.selected];
    spaced_text(b->chapter,inner,y,app.mono_small,GOLD,1);y+=33;
    TTF_Font *namefont=app.serif_italic;int namew=pw-pad*2;if(text_width(b->name,namefont)>namew)namefont=app.serif_title_medium;if(text_width(b->name,namefont)>namew)namefont=app.serif_title_small;if(text_width(b->name,namefont)>namew)namefont=app.serif_title_tiny;
    if(text_width(b->name,namefont)>namew){y=wrapped_text(b->name,inner,y,namew,namefont,INK,23)+12;}else{text_raw(b->name,inner,y,namefont,INK);y+=(namefont==app.serif_italic)?64:42;}
    y=wrapped_text(b->epigraph,inner,y,pw-pad*2,app.serif_small_italic,(SDL_Color){201,169,110,245},27);y+=18;
    line(inner,y,app.w-pad,y,(SDL_Color){201,169,110,65});y+=19;
    y=wrapped_text(b->prose,inner,y,pw-pad*2,app.serif,(SDL_Color){216,210,192,238},22);y+=24;
    line(inner,y,app.w-pad,y,(SDL_Color){201,169,110,65});y+=12;
    for(int row=0;row<3;row++){for(int col=0;col<2;col++){int k=row*2+col;if(k>=6)continue;int cx=inner+col*(pw-pad*2)/2;text_raw(b->data[k].label,cx,y,app.mono_small,MUTED);char val[160];snprintf(val,sizeof(val),"%s%s%s",b->data[k].value,b->data[k].unit[0]?" ":"",b->data[k].unit);text_raw(val,cx,y+17,app.serif,INK);}y+=55;line(inner,y-3,app.w-pad,y-3,(SDL_Color){201,169,110,32});}
    SDL_RenderSetClipRect(app.renderer,NULL);text_raw("×",app.w-42,20,app.serif, GOLD);
}
static void draw_footer(void){int n=count_view(),slot=selected_slot();char count[32];snprintf(count,sizeof(count),"%02d  /  %02d",slot+1,n);spaced_text(count,34,app.h-43,app.mono_small,INK,1);
    int start=app.w/2-((n>8?8:n)*30)/2;int per=n>8?2:1;for(int i=0;i<n;i++){int x=start+i*30*per;int idx=view_index(i);const Body *b=&bodies[idx];int r=clampi(b->dot_size/2,2,7);filled_circle(x,app.h-37,r,idx==app.selected?GOLD:rgb(b->color,190));if(idx==app.selected)circle_outline(x,app.h-37,r+4,(SDL_Color){201,169,110,110});}
    draw_button(app.w-134,app.h-59,44,40,"←",false);draw_button(app.w-79,app.h-59,44,40,"→",false);
    if(app.view==VIEW_PLANETS)text_raw("DRAG TO ROTATE  ·  SCROLL TO ZOOM  ·  SELECT A WORLD",app.w/2-190,app.h-22,app.mono_small,(SDL_Color){138,133,120,170});
}
static void draw_intro(void){draw_backdrop();SDL_Color dim={0,0,0,105};fill_rect(0,0,app.w,app.h,dim);const char *ey="A VISUAL ESSAY  ·  MMXXVI";int ew=text_width(ey,app.mono_small);spaced_text(ey,(app.w-ew)/2-15,app.h/2-160,app.mono_small,GOLD,2);TTF_Font *title=app.w<520?app.serif_title_small:app.serif_italic;const char *l1="Field Notes";const char *l2="from the Outer Dark";int w1=text_width(l1,title),w2=text_width(l2,title);text_raw(l1,(app.w-w1)/2,app.h/2-105,title,INK);text_raw(l2,(app.w-w2)/2,app.h/2-25,title,GOLD);const char *sub=app.w<520?"FROM THE SUN TO THE LOCAL GROUP":"FROM THE SOLAR SYSTEM TO THE EDGE OF THE LOCAL GROUP";spaced_text(sub,(app.w-text_width(sub,app.mono_small))/2-10,app.h/2+70,app.mono_small,MUTED,1);draw_button(app.w/2-115,app.h/2+130,230,48,"BEGIN TRANSMISSION  →",true);spaced_text("VOL. I  ·  FIELD TRANSMISSION",(app.w-250)/2,app.h/2+216,app.mono_small,FAINT,1);}
static void render(void){if(!app.entered){draw_intro();SDL_RenderPresent(app.renderer);return;}draw_backdrop();draw_masthead();if(app.view==VIEW_PLANETS||app.view==VIEW_DWARFS||app.view==VIEW_MILKYWAY||app.view==VIEW_GALAXIES)draw_orbit_scene();draw_data_panel();draw_footer();SDL_RenderPresent(app.renderer);}
static void select_at(int x,int y){int panel=panel_width();if(app.view==VIEW_GALAXIES){int cx=(app.w-panel)/2,cy=app.h/2;static const float pos[6][2]={{-.65f,-.25f},{.15f,-.35f},{.68f,-.13f},{-.48f,.32f},{.45f,.35f},{-.08f,.12f}};float bd=1e9f;int bi=-1;for(int i=0;i<6;i++){float dx=x-(cx+pos[i][0]*app.w*.40f),dy=y-(cy+pos[i][1]*app.h*.52f),d=dx*dx+dy*dy;if(d<bd){bd=d;bi=i;}}if(bd<10000){app.selected=14+bi;app.info_open=true;app.scroll=0;}return;}
    if(app.view==VIEW_MILKYWAY){int cx=(app.w-panel)/2,cy=app.h/2;int r=(int)(fminf((app.w-panel)*.40f,app.h*.36f)*app.zoom);if((x-cx)*(x-cx)+(y-cy)*(y-cy)<r*r){app.selected=13;app.info_open=true;}return;}
    int cx=(app.w-panel)/2,cy=app.h/2;float maxd=app.view==VIEW_PLANETS?102:130,scale=fminf((app.w-panel)*.43f/maxd,app.h*.36f/maxd)*app.zoom;int first=app.view==VIEW_PLANETS?0:8,last=app.view==VIEW_PLANETS?8:13;float best=1e9f;int hit=-1;for(int i=first;i<last;i++){float fx,fy;int r=planet_screen(&bodies[i],scale,cx,cy,&fx,&fy);float dx=x-fx,dy=y-fy,d=dx*dx+dy*dy;if(d<(r+11)*(r+11)&&d<best){best=d;hit=i;}}if(hit>=0){app.selected=hit;app.info_open=true;app.scroll=0;}}
static void handle_click(int x,int y){if(!app.entered){if(inside(x,y,app.w/2-115,app.h/2+130,230,48)){app.entered=true;change_view(VIEW_PLANETS);}return;}if(y>=88&&y<=118){int bx=masthead_left();for(int i=0;i<4;i++){int bw=view_button_width(i);if(inside(x,y,bx,88,bw,30)){change_view((View)i);return;}bx+=bw+5;}}if(app.info_open&&x>=app.w-60&&y<62){app.info_open=false;app.scroll=0;return;}
    if(y>app.h-66){if(x>app.w-145&&x<app.w-85){step_selection(-1);return;}if(x>app.w-85){step_selection(1);return;}if(x>app.w*.25f&&x<app.w*.75f){int n=count_view();int start=app.w/2-(n>8?8:n)*15;int k=(x-start)/30;if(k>=0&&k<n){app.selected=view_index(k);app.info_open=true;app.scroll=0;}return;}}
    if(app.info_open&&x>=app.w-panel_width()){return;}select_at(x,y);}
static void events(void){SDL_Event e;while(SDL_PollEvent(&e)){if(e.type==SDL_QUIT)app.running=false;
        if(e.type==SDL_WINDOWEVENT&&e.window.event==SDL_WINDOWEVENT_SIZE_CHANGED){app.w=e.window.data1;app.h=e.window.data2;}
        if(e.type==SDL_MOUSEMOTION){app.mouse_x=e.motion.x;app.mouse_y=e.motion.y;if(app.dragging){app.camera_rotation-=e.motion.xrel*.006f;app.tilt=clampf(app.tilt-e.motion.yrel*.0005f,.08f,.65f);}}
        if(e.type==SDL_MOUSEBUTTONDOWN&&e.button.button==SDL_BUTTON_LEFT){app.drag_x=e.button.x;app.drag_y=e.button.y;app.dragging=app.entered&&e.button.y>120&&e.button.y<app.h-70;}
        if(e.type==SDL_MOUSEBUTTONUP&&e.button.button==SDL_BUTTON_LEFT){int dx=e.button.x-app.drag_x,dy=e.button.y-app.drag_y;bool click=abs(dx)<5&&abs(dy)<5;app.dragging=false;if(click)handle_click(e.button.x,e.button.y);}
        if(e.type==SDL_MOUSEWHEEL&&app.entered){if(app.info_open&&app.mouse_x>app.w-panel_width())app.scroll=clampf(app.scroll-e.wheel.y*50,0,900);else app.zoom=clampf(app.zoom*(e.wheel.y>0?1.12f:.89f),.62f,1.9f);}
        if(e.type==SDL_KEYDOWN){SDL_Keycode k=e.key.keysym.sym;if(k==SDLK_ESCAPE){if(app.info_open)app.info_open=false;else if(app.entered)app.entered=false;else app.running=false;}else if(app.entered&&(k==SDLK_RIGHT||k==SDLK_SPACE))step_selection(1);else if(app.entered&&k==SDLK_LEFT)step_selection(-1);else if(app.entered&&k==SDLK_1)change_view(VIEW_PLANETS);else if(app.entered&&k==SDLK_2)change_view(VIEW_DWARFS);else if(app.entered&&k==SDLK_3)change_view(VIEW_MILKYWAY);else if(app.entered&&k==SDLK_4)change_view(VIEW_GALAXIES);}
    }}
static bool init_app(bool smoke){memset(&app,0,sizeof(app));app.running=true;app.smoke=smoke;app.w=1440;app.h=900;app.tilt=.20f;app.zoom=1;app.view=VIEW_PLANETS;stars_init();if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)!=0){fprintf(stderr,"SDL init: %s\n",SDL_GetError());return false;}if(TTF_Init()!=0){fprintf(stderr,"SDL_ttf init: %s\n",TTF_GetError());return false;}Uint32 flags=SDL_WINDOW_RESIZABLE|(smoke?SDL_WINDOW_HIDDEN:SDL_WINDOW_SHOWN);app.window=SDL_CreateWindow("Field Notes from the Outer Dark",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,app.w,app.h,flags);if(!app.window){fprintf(stderr,"Window: %s\n",SDL_GetError());return false;}SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");app.renderer=SDL_CreateRenderer(app.window,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);if(!app.renderer)app.renderer=SDL_CreateRenderer(app.window,-1,SDL_RENDERER_SOFTWARE);if(!app.renderer){fprintf(stderr,"Renderer: %s\n",SDL_GetError());return false;}SDL_SetRenderDrawBlendMode(app.renderer,SDL_BLENDMODE_BLEND);
    app.serif=TTF_OpenFont(FONT_SERIF,18);app.serif_italic=TTF_OpenFont(FONT_ITALIC,58);if(!app.serif_italic)app.serif_italic=TTF_OpenFont(FONT_SERIF_ITALIC,58);app.serif_title_medium=TTF_OpenFont(FONT_ITALIC,38);app.serif_title_small=TTF_OpenFont(FONT_ITALIC,29);app.serif_title_tiny=TTF_OpenFont(FONT_ITALIC,18);app.serif_small_italic=TTF_OpenFont(FONT_ITALIC,22);if(!app.serif_small_italic)app.serif_small_italic=TTF_OpenFont(FONT_SERIF_ITALIC,22);app.mono=TTF_OpenFont(FONT_MONO,12);app.mono_small=TTF_OpenFont(FONT_MONO,10);if(!app.serif||!app.serif_italic||!app.serif_title_medium||!app.serif_title_small||!app.serif_title_tiny||!app.serif_small_italic||!app.mono||!app.mono_small){fprintf(stderr,"Font load failed: %s\n",TTF_GetError());return false;}for(int i=0;i<BODY_COUNT;i++)app.surfaces[i]=make_planet_surface(&bodies[i],i);return true;}
static void cleanup(void){for(int i=0;i<BODY_COUNT;i++)if(app.surfaces[i])SDL_DestroyTexture(app.surfaces[i]);if(app.serif)TTF_CloseFont(app.serif);if(app.serif_italic)TTF_CloseFont(app.serif_italic);if(app.serif_title_medium)TTF_CloseFont(app.serif_title_medium);if(app.serif_title_small)TTF_CloseFont(app.serif_title_small);if(app.serif_title_tiny)TTF_CloseFont(app.serif_title_tiny);if(app.serif_small_italic)TTF_CloseFont(app.serif_small_italic);if(app.mono)TTF_CloseFont(app.mono);if(app.mono_small)TTF_CloseFont(app.mono_small);if(app.renderer)SDL_DestroyRenderer(app.renderer);if(app.window)SDL_DestroyWindow(app.window);TTF_Quit();SDL_Quit();}
static void smoke_snapshot(void){SDL_Surface *s=SDL_CreateRGBSurfaceWithFormat(0,app.w,app.h,32,SDL_PIXELFORMAT_ARGB8888);if(!s)return;if(SDL_RenderReadPixels(app.renderer,NULL,SDL_PIXELFORMAT_ARGB8888,s->pixels,s->pitch)==0)SDL_SaveBMP(s,"/tmp/space-c-smoke.bmp");SDL_FreeSurface(s);}
#ifdef __EMSCRIPTEN__
static EM_BOOL browser_resize(int event_type,const EmscriptenUiEvent *event,void *user_data){(void)event_type;(void)user_data;if(event){app.w=event->windowInnerWidth;app.h=event->windowInnerHeight;}else{double w=1440,h=900;if(emscripten_get_element_css_size("#canvas",&w,&h)==EMSCRIPTEN_RESULT_SUCCESS){app.w=(int)w;app.h=(int)h;}}if(app.w<320)app.w=320;if(app.h<240)app.h=240;emscripten_set_canvas_element_size("#canvas",app.w,app.h);if(app.window)SDL_SetWindowSize(app.window,app.w,app.h);return EM_TRUE;}
static void browser_frame(void){if(!app.running){emscripten_cancel_main_loop();return;}events();static Uint64 last=0;Uint64 now=SDL_GetPerformanceCounter();if(!last)last=now;double dt=(double)(now-last)/(double)SDL_GetPerformanceFrequency();last=now;app.time+=(float)fmin(dt,.05);render();}
int main(void){bool smoke=false;
#else
int main(int argc,char **argv){bool smoke=argc>1&&!strcmp(argv[1],"--smoke");
#endif
if(smoke){SDL_setenv("SDL_VIDEODRIVER","dummy",1);}if(!init_app(smoke)){cleanup();return 1;}if(smoke){app.entered=true;for(int v=0;v<4;v++){change_view((View)v);for(int slot=0;slot<count_view();slot++){app.selected=view_index(slot);app.time+=1.0f/60;render();}}change_view(VIEW_PLANETS);render();smoke_snapshot();printf("smoke ok: all %d catalogue bodies and 4 views rendered; frame saved to /tmp/space-c-smoke.bmp\n",BODY_COUNT);cleanup();return 0;}
#ifdef __EMSCRIPTEN__
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,NULL,EM_FALSE,browser_resize);browser_resize(0,NULL,NULL);emscripten_set_main_loop(browser_frame,0,1);return 0;
#else
    Uint64 last=SDL_GetPerformanceCounter();while(app.running){events();Uint64 now=SDL_GetPerformanceCounter();double dt=(double)(now-last)/(double)SDL_GetPerformanceFrequency();last=now;app.time+=(float)fmin(dt,.05);render();}cleanup();return 0;
#endif
}
