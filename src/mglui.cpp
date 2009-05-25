#include "mglui.h"

#ifdef __cplusplusx

mglui *mglui::m_pInstance = NULL;

mglui *mglui::Instance() {
	if (!m_pInstance) {
		m_pInstance = new mglui;
	}

	return m_pInstance;
}



/* defining callbacks for glui */

void myGlutKeyboard(unsigned char Key, int x, int y)
{
  glutPostRedisplay();
}

void myGlutMenu( int value )
{
  myGlutKeyboard( value, 0, 0 );
}

void myGlutIdle( void ) {
  /* According to the GLUT specification, the current window is 
     undefined during an idle callback.  So we need to explicitly change
     it if necessary 
  if ( glutGetWindow() != main_window ) 
    glutSetWindow(main_window);  
*/
  /*  GLUI_Master.sync_live_all();  -- not needed - nothing to sync in this
                                       application  */

  glutPostRedisplay();
}

void myGlutMouse(int button, int button_state, int x, int y ) {
}

void myGlutMotion(int x, int y ) {
  glutPostRedisplay(); 
}

void myGlutReshape( int x, int y ) {
}

void setupGLUI() {
	GLUI *glui, *glui2;
	
	int main_window = glutCreateWindow( "GLUI Example 5" );
	glutHideWindow();

  GLUI_Master.set_glutReshapeFunc( myGlutReshape );  
  GLUI_Master.set_glutKeyboardFunc( myGlutKeyboard );
  GLUI_Master.set_glutSpecialFunc( NULL );
  GLUI_Master.set_glutMouseFunc( myGlutMouse );
  glutMotionFunc( myGlutMotion );
	glutDisplayFunc(graphics_step);
	
	glui = GLUI_Master.create_glui_subwindow( main_window, 
					    GLUI_SUBWINDOW_RIGHT );

glui->set_main_gfx_window(main_window);


  GLUI_Rollout *obj_panel = new GLUI_Rollout(glui, "Properties", false );
}


#endif
