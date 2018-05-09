#include <stdio.h>
#include <portaudio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>


#include <gtk/gtk.h>

#include "utils/util.h"
#include "params.h"
#include "control/control.h"
#include "audio/audio.h"

static ControlStatus control_status;
pthread_t controlThread;

GtkObject *ampAdj[WAVE_COUNT];
GtkObject *phaseAdj[WAVE_COUNT];

static void start(
    GtkWidget *widget,
    gpointer data)
{
    if (control_status.engineRunning != 1) {
        pthread_create( &controlThread, NULL, (void *) &initEngine, &control_status );
        printf("Engine started\n");
    } else {
        printf("Engine is running already\n");
    }
    control_status.engineRunning = 1;
}

static void stop(
    GtkWidget *widget,
    gpointer data ) {
    if (control_status.engineRunning == 1) {
        closeAudioStream();
        printf("Engine stopped\n");
    } else {
        printf("Engine not started\n");
    }
    control_status.engineRunning = 0;
}

static void phaseAdjustment(
    GtkWidget *widget,
    gpointer data ) {
    control_status.phase[(int) data]    = (float) gtk_range_get_adjustment(widget)->value;
}

static void ampAdjustment(
    GtkWidget *widget,
    gpointer data ) {
    control_status.ampWave[(int) data]  = (float) gtk_range_get_adjustment(widget)->value;
}

/* Exit the application */
static void destroy(
    GtkWidget *widget,
    gpointer data )
{
    stop(widget, data);
    gtk_main_quit ();
    exit(1);
}

static gboolean closeGUI( GtkWidget *widget,
                              GdkEvent  *event,
                              gpointer   data )
{
    /* If you return FALSE in the "delete-event" signal handler,
     * GTK will emit the "destroy" signal. Returning TRUE means
     * you don't want the window to be destroyed.
     * This is useful for popping up 'are you sure you want to quit?'
     * type dialogs. */

     if (control_status.engineRunning == 1) {
         closeAudioStream();
         control_status.engineRunning = 0;
     }

     g_print ("delete event occurred\n");

     /* Change TRUE to FALSE and the main window will be destroyed with
     * a "delete-eveintnt". */

     return FALSE;
}

int main( int   argc,
          char *argv[] )
{
    /* init gtk widgets */
    GtkWidget *window;
    GtkWidget *windowTable;
    GtkWidget *startEngine;
    GtkWidget *stopEngine;
    GtkWidget *ampScale[WAVE_COUNT];
    GtkWidget *phaseScale[WAVE_COUNT];

    readToWavetable(0, "../../waveforms/sinus.csv");

    /* init gtk */
    gtk_init(&argc, &argv);

    /* create a new window and its table */
    window          = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    windowTable     = gtk_table_new(20, 20, FALSE);

    /* Set the window title */
    gtk_window_set_title (GTK_WINDOW (window), "Phase Synth");

    gtk_container_add (GTK_CONTAINER (window), windowTable);
    /* Add the close callback to the delete event */
    g_signal_connect(window, "delete-event",
		      G_CALLBACK (closeGUI), NULL);

    /* Add the destroy callback to the destroy event */
    g_signal_connect (window, "destroy",
		      G_CALLBACK (destroy), NULL);

    /* Set the border width of the window. */
    gtk_container_set_border_width(GTK_CONTAINER (window), 10);

    /* Create the turn off/on buttons */
    startEngine     = gtk_button_new_with_label ("Start Engine");
    stopEngine      = gtk_button_new_with_label ("Stop Engine");

    g_signal_connect(startEngine, "clicked", G_CALLBACK (start), NULL);
    g_signal_connect(stopEngine,  "clicked", G_CALLBACK (stop),  NULL);

    /* An the buttons to the table */
    gtk_table_attach_defaults((GtkTable *) windowTable, startEngine, 0, 1, 0, 2);
    gtk_table_attach_defaults((GtkTable *) windowTable, stopEngine,  1, 2, 0, 2);

    /* init phase/amp adjustments */
    for (int waveAdjIndex = 0; waveAdjIndex < WAVE_COUNT; waveAdjIndex++) {
        ampAdj[waveAdjIndex]      = gtk_adjustment_new(0, 0, VALUE_RANGE, 1, 1, 0);
        phaseAdj[waveAdjIndex]    = gtk_adjustment_new(0, 0, VALUE_RANGE, 1, 1, 0);

        ampScale[waveAdjIndex]    = gtk_hscale_new((GtkAdjustment *) ampAdj[waveAdjIndex]);
        phaseScale[waveAdjIndex]  = gtk_hscale_new((GtkAdjustment *) phaseAdj[waveAdjIndex]);

        gtk_range_set_update_policy((GtkRange *) ampScale[waveAdjIndex],   GTK_UPDATE_CONTINUOUS);
        gtk_range_set_update_policy((GtkRange *) phaseScale[waveAdjIndex], GTK_UPDATE_CONTINUOUS);

        g_signal_connect(ampScale[waveAdjIndex],    "value_changed", G_CALLBACK (ampAdjustment),   (gpointer)  waveAdjIndex);
        g_signal_connect(phaseScale[waveAdjIndex],  "value_changed", G_CALLBACK (phaseAdjustment), (gpointer)  waveAdjIndex);

        gtk_table_attach_defaults((GtkTable *) windowTable, ampScale[waveAdjIndex],   0, 1, 3*waveAdjIndex+3, 3*waveAdjIndex+4);
        gtk_table_attach_defaults((GtkTable *) windowTable, phaseScale[waveAdjIndex], 1, 2, 3*waveAdjIndex+3, 3*waveAdjIndex+4);

        gtk_widget_show(ampScale[waveAdjIndex]);
        gtk_widget_show(phaseScale[waveAdjIndex]);
    }

    /* Display the widgets in the window */
    gtk_widget_show(startEngine);
    gtk_widget_show(stopEngine);

    /* the tables */
    gtk_widget_show(windowTable);

    /* and the window */
    gtk_widget_show(window);

    /* Start GTK */
    gtk_main();

    return 0;
}
