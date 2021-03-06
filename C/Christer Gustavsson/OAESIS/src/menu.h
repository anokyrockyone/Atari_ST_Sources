#ifndef __MENU__
#define __MENU__

#include "types.h"


/****************************************************************************
 *  Menu_init_module                                                        *
 *   Initiate menu module.                                                  *
 ****************************************************************************/
WORD                    /*                                                  */
Menu_init_module(void); /*                                                  */
/****************************************************************************/

/****************************************************************************
 *  Menu_exit_module                                                        *
 *   Shutdown menu module.                                                  *
 ****************************************************************************/
void                    /*                                                  */
Menu_exit_module(void); /*                                                  */
/****************************************************************************/

/****************************************************************************
 *  Menu_handler                                                            *
 *   Handle main menu events.                                               *
 ****************************************************************************/
void                   /*                                                   */
Menu_handler(void);    /*                                                   */
/****************************************************************************/

/****************************************************************************
 * Menu_bar_remove                                                          *
 *  Remove menu.                                                            *
 ****************************************************************************/
WORD              /*                                                        */
Menu_bar_remove(  /*                                                        */
WORD apid);       /* Application whose menu is to be removed.               */
/****************************************************************************/

/****************************************************************************
 * Menu_do_register                                                         *
 *  Implementation of menu_register().                                      *
 ****************************************************************************/
WORD              /* Menu identification, or -1.                            */
Menu_do_register( /*                                                        */
WORD apid,        /* Application id, or -1.                                 */
BYTE *title);     /* Title to register application under.                   */
/****************************************************************************/

/****************************************************************************
 * Menu_unregister                                                          *
 *  Remove menu entry of application.                                       *
 ****************************************************************************/
void              /* Menu identification, or -1.                            */
Menu_unregister(  /*                                                        */
WORD apid);       /* Application id.                                        */
/****************************************************************************/

/****************************************************************************
 * Menu_update_appl                                                         *
 *  Update the application menu.                                            *
 ****************************************************************************/
WORD                    /* Top application id.                              */
Menu_update_appl(void); /*                                                  */
/****************************************************************************/

/****************************************************************************
 *  Menu_bar                                                                *
 *   0x001e menu_bar() library code.                                        *
 ****************************************************************************/
void              /*                                                        */
Menu_bar(         /*                                                        */
AES_PB *apb);     /* Pointer to AES parameter block.                        */
/****************************************************************************/

/****************************************************************************
 *  Menu_icheck                                                             *
 *   0x001f menu_icheck().                                                  *
 ****************************************************************************/
void              /*                                                        */
Menu_icheck(      /*                                                        */
AES_PB *apb);     /* Pointer to AES parameter block.                        */
/****************************************************************************/

/****************************************************************************
 *  Menu_ienable                                                            *
 *   0x0020 menu_ienable().                                                 *
 ****************************************************************************/
void              /*                                                        */
Menu_ienable(     /*                                                        */
AES_PB *apb);     /* Pointer to AES parameter block.                        */
/****************************************************************************/

/****************************************************************************
 *  Menu_tnormal                                                            *
 *   0x0021 menu_tnormal().                                                 *
 ****************************************************************************/
void              /*                                                        */
Menu_tnormal(     /*                                                        */
AES_PB *apb);     /* Pointer to AES parameter block.                        */
/****************************************************************************/

/****************************************************************************
 *  Menu_text                                                               *
 *   0x0022 menu_text().                                                    *
 ****************************************************************************/
void              /*                                                        */
Menu_text(        /*                                                        */
AES_PB *apb);     /* Pointer to AES parameter block.                        */
/****************************************************************************/

/****************************************************************************
 *  Menu_register                                                           *
 *   0x0023 menu_register().                                                *
 ****************************************************************************/
void              /*                                                        */
Menu_register(    /*                                                        */
AES_PB *apb);     /* Pointer to AES parameter block.                        */
/****************************************************************************/

#endif
