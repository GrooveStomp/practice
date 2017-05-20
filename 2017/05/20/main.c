#include <stdio.h> /* printf */
#include <stdlib.h> /* exit */
#include <stdbool.h> /* booleans */
#include <unistd.h> /* sleep */

#include <sys/ipc.h> /* inter-process communication */
#include <sys/shm.h> /* shared memory */

#include <xcb/xcb.h>
#include <xcb/shm.h>
#include <xcb/xcb_image.h>

int main(int argc, char* argv[])
{
        xcb_connection_t *conn;
        const xcb_setup_t *setup;
        xcb_screen_t *screen;
        xcb_window_t window_id;
        uint32_t value_mask;
        uint32_t values[2];

        conn = xcb_connect(NULL, NULL);
        if(xcb_connection_has_error(conn))
        {
                printf("Error opening display\n");
                exit(1);
        }

        setup = xcb_get_setup(conn);
        screen = xcb_setup_roots_iterator(setup).data;

        window_id = xcb_generate_id(conn);
        value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
        values[0] = screen->white_pixel;
        values[1] = XCB_EVENT_MASK_EXPOSURE |
                XCB_EVENT_MASK_BUTTON_PRESS |
                XCB_EVENT_MASK_KEY_PRESS;

        xcb_create_window(
                conn,
                screen->root_depth,
                window_id,
                screen->root,
                0, 0,
                100, 100,
                1,
                XCB_WINDOW_CLASS_INPUT_OUTPUT,
                screen->root_visual,
                value_mask,
                values);

        xcb_gcontext_t gcontext;
        value_mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
        values[0] = screen->black_pixel;
        values[1] = 0;
        gcontext = xcb_generate_id(conn);
        xcb_create_gc(conn, gcontext, window_id, value_mask, values);

        xcb_map_window(conn, window_id);
        xcb_flush(conn);

        // Shared memory pixmap test.
        xcb_shm_query_version_reply_t *reply;
        xcb_shm_segment_info_t info;

        reply = xcb_shm_query_version_reply(
                conn,
                xcb_shm_query_version(conn),
                NULL);

        if(!reply)
        {
                printf("Shm error\n");
                exit(1);
        }

        info.shmid = shmget(IPC_PRIVATE, 100*100*4, IPC_CREAT | 0777);
        info.shmaddr = shmat(info.shmid, 0, 0);
        info.shmseg = xcb_generate_id(conn);
        xcb_shm_attach(conn, info.shmseg, info.shmid, 0);
        shmctl(info.shmid, IPC_RMID, 0);

        uint32_t *data = (int32_t*)(info.shmaddr);
        xcb_pixmap_t pix = xcb_generate_id(conn);
        xcb_shm_create_pixmap(
                conn,
                pix,
                window_id,
                100, 100,
                screen->root_depth,
                info.shmseg,
                0);

        for(int i=0; i < 100*100; i++)
        {
                usleep(500);
                data[i] = 0xFFFFFF;
                xcb_copy_area(
                        conn,
                        pix,
                        window_id,
                        gcontext,
                        0,0,0,0,
                        100, 100);

                xcb_flush(conn);
        }

        bool running = true;
        while(running)
        {
                xcb_generic_event_t *event = xcb_poll_for_event(conn);
                if(!event) { continue; }

                switch(event->response_type)
                {
                        case XCB_KEY_PRESS:
                        {
                                xcb_key_press_event_t *keypress = (xcb_key_press_event_t *)event;
                                if(keypress->detail == 24) // "q" key
                                {
                                        running = false;
                                }
                                else
                                {
                                        printf("Keycode: %d\n", ((xcb_key_press_event_t *)event)->detail);
                                }
                        }
                        break;

                        case XCB_BUTTON_PRESS:
                        {
                        }
                        break;

                        case XCB_EXPOSE:
                        {
                        }
                        break;

                        default:
                        {
                        }
                        break;
                }

                free(event);
        }

        xcb_shm_detach(conn, info.shmseg);
        shmdt(info.shmaddr);
        xcb_free_pixmap(conn, pix);
        xcb_disconnect(conn);
        exit(0);
}
