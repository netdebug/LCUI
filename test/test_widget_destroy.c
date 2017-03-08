#include <LCUI_Build.h>
#include <LCUI/LCUI.h>
#include <LCUI/timer.h>
#include <LCUI/gui/widget.h>
#include <LCUI/gui/widget/textview.h>

static LCUI_Widget target = NULL;

typedef struct DialogContextRec_ {
	LCUI_BOOL result;
	LCUI_MainLoop loop;
} DialogContextRec, *DialogContext;

static void OnBtnOkClick( LCUI_Widget w, LCUI_WidgetEvent e, void *arg )
{
	DialogContext ctx = e->data;
	ctx->result = TRUE;
	LCUIMainLoop_Quit( ctx->loop );
}

static void OnBtnCancelClick( LCUI_Widget w, LCUI_WidgetEvent e, void *arg )
{
	DialogContext ctx = e->data;
	ctx->result = FALSE;
	_DEBUG_MSG( "on click, widget: %p, e->target: %p\n", w, e->target );
	LCUIMainLoop_Quit( ctx->loop );
}

static LCUI_BOOL LCUIDialog_Confirm( LCUI_Widget parent,
				     const wchar_t* title, const wchar_t *text )
{
	DialogContextRec ctx = { 0 };
	LCUI_Widget dialog_text, box;
	LCUI_Widget dialog = LCUIWidget_New( NULL );
	LCUI_Widget dialog_body = LCUIWidget_New( NULL );
	LCUI_Widget dialog_content = LCUIWidget_New( NULL );
	LCUI_Widget dialog_header = LCUIWidget_New( NULL );
	LCUI_Widget dialog_footer = LCUIWidget_New( NULL );
	LCUI_Widget btn_cancel = LCUIWidget_New( NULL );
	LCUI_Widget btn_ok = LCUIWidget_New( NULL );
	ctx.loop = LCUIMainLoop_New();
	Widget_AddClass( dialog, "dialog" );
	Widget_AddClass( dialog_body, "dialog-body" );
	Widget_AddClass( dialog_content, "dialog-content" );
	Widget_AddClass( dialog_header, "dialog-header" );
	Widget_AddClass( dialog_footer, "dialog-footer" );
	dialog_text = LCUIWidget_New( "textview" );
	TextView_SetTextW( dialog_text, title );
	Widget_Append( dialog_header, dialog_text );
	dialog_text = LCUIWidget_New( "textview" );
	TextView_SetTextW( dialog_text, text );
	Widget_Append( dialog_body, dialog_text );
	box = LCUIWidget_New( NULL );
	dialog_text = LCUIWidget_New( "textview" );
	Widget_AddClass( box, "dialog-btn-group" );
	Widget_AddClass( btn_ok, "dialog-btn" );
	Widget_AddClass( dialog_text, "text" );
	TextView_SetTextW( dialog_text, L"OK" );
	Widget_Append( btn_ok, dialog_text );
	Widget_Append( box, btn_ok );
	Widget_Append( dialog_footer, box );
	box = LCUIWidget_New( NULL );
	dialog_text = LCUIWidget_New( "textview" );
	Widget_AddClass( box, "dialog-btn-group" );
	Widget_AddClass( btn_cancel, "dialog-btn" );
	Widget_AddClass( dialog_text, "text" );
	TextView_SetTextW( dialog_text, L"Cancel" );
	target = dialog_text;
	_DEBUG_MSG( "debug target widget: %p\n", target );
	Widget_Append( btn_cancel, dialog_text );
	Widget_Append( box, btn_cancel );
	Widget_Append( dialog_footer, box );
	Widget_Append( dialog_content, dialog_header );
	Widget_Append( dialog_content, dialog_body );
	Widget_Append( dialog_content, dialog_footer );
	Widget_Append( dialog, dialog_content );
	Widget_Append( parent, dialog );
	Widget_BindEvent( btn_ok, "click", OnBtnOkClick, &ctx, NULL );
	Widget_BindEvent( btn_cancel, "click", OnBtnCancelClick, &ctx, NULL );
	LCUIMainLoop_Run( ctx.loop );
	Widget_Destroy( dialog );
	return ctx.result;
}

static void TimerForClick( void *arg )
{
	LCUI_WidgetEventRec ev = { 0 };
	ev.type = WET_CLICK;
	ev.target = target;
	Widget_PostEvent( target, &ev, NULL, NULL );
}

static void TestDialog( void *arg1, void *arg2)
{
	LCUI_Widget root = LCUIWidget_GetRoot();
	LCUIDialog_Confirm( root, L"test title", L"test text" );
	LCUI_Quit();
}

static void TimerForDialog( void *arg )
{
	LCUI_PostSimpleTask( TestDialog, NULL, NULL );
}

int test_widget_destroy( void )
{
	LCUI_Init();
	LCUITimer_Set( 1000, TimerForDialog, NULL, FALSE );
	LCUITimer_Set( 3000, TimerForClick, NULL, FALSE );
	return LCUI_Main();
}
