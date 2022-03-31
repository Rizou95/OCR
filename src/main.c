#include <err.h>
#include <gtk/gtk.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_rotozoom.h"
#include "utils/sdl_utils.h"
#include "color/color_suppr.h"
#include "Hough/segmentation.h"
#include "Hough/detection.h"
#include "solver/sudoku.h"
#include "network/network.h"

typedef struct UI
{
    GtkWindow* window;
    GtkButton* clear_button;
    GtkFileChooserButton* file_chooser;
    GtkScale* rotation_scale;
    GtkSwitch* rotation_auto;
    GtkButton* start_button;
    GtkImage* image;
    GtkProgressBar* progress_bar;
    GtkEntry* output_name;
    GtkTextView* terminal_window;
    int number;
} UI;

typedef struct OCR
{
    SDL_Surface* image_surface;
    SDL_Surface* image_surface_rotated;
    gboolean auto_rot;
    UI* ui;
    int sudoku[9][9];
} OCR;

void write_terminal(OCR *ocr, char *str)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ocr->ui->terminal_window));
    gtk_text_buffer_insert_at_cursor(buffer, str, -1);
}

void rebuild_image(OCR *ocr)
{
    SDL_Surface *rebuilded_image = SDL_CreateRGBSurface(SDL_HWSURFACE, 31 * 9, 31 * 9, 32, 0, 0, 0, 0);
    for (size_t y = 0; y < 9; y++)
    {
        for (size_t x = 0; x < 9; x++)
        {
            SDL_Rect dstrect;
            dstrect.x = x * 31 + 3;
            dstrect.y = y * 31 + 3;
            dstrect.w = 28;
            dstrect.h = 28;
            SDL_Surface* number = find_number(ocr->sudoku[y][x]);
            SDL_BlitSurface(number, NULL, rebuilded_image, &dstrect);
        }
    }
    //SDL_SaveBMP(rebuilded_image, "./images/rebuilded.bmp");
    SDL_FreeSurface(ocr->image_surface);
    ocr->image_surface = rebuilded_image;
}

void save_image(OCR* ocr)
{
    char str[1024];
    if (gtk_entry_get_text_length(ocr->ui->output_name) == 0)
        snprintf(str, sizeof(str), "./images/output.bmp");
    else
        snprintf(str, sizeof(str), "./images/%s.bmp", gtk_entry_get_text(ocr->ui->output_name));
    write_terminal(ocr, "File saved as:\n");
    write_terminal(ocr, str);
    write_terminal(ocr, "\n");
    SDL_SaveBMP(ocr->image_surface ,str);
    gtk_image_set_from_file(GTK_IMAGE(ocr->ui->image), str);
}

void preprocessing(OCR *ocr)
{   
    SDL_Surface *image_surface = ocr->image_surface;
    gaussian_blur(image_surface, 1);
    greyscale(image_surface);
    contrast_change(image_surface, 30);
    up_blacks(image_surface);
    up_blacks(image_surface);
    blackNwhite(image_surface);
    //invert(image_surface);
}

int find_image(gchar *str)
{
    for (gchar *i = str; *i != 0; i++)
    {
        if (*i >= '0' && *i <= '9')
        {
            return *i - '0';
        }
    }
    return -1;
}

gboolean on_output_name_changed(GtkEntry* entry, gpointer user_data)
{
    OCR *ocr = user_data;
    ocr->ui->output_name = entry;
    
    return FALSE;
}

gboolean on_scale_change(GtkRange *range, GtkScrollType *scroll, gdouble value, gpointer user_data)
{
    OCR *ocr = user_data;
    if (ocr->image_surface != NULL)
    {
        value = CLAMP((int)value, 0, 360);
        ocr->image_surface_rotated = resize(SDL_RotationCentralN(ocr->image_surface, value));
        SDL_SaveBMP(ocr->image_surface_rotated, "./tmp/rot.bmp");
        gtk_image_set_from_file(GTK_IMAGE(ocr->ui->image), "./tmp/rot.bmp");
    }
    return FALSE;
}

gboolean on_file_chooser_file_set(GtkFileChooserButton *file_chooser, gpointer user_data)
{
    OCR *ocr = user_data;
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(ocr->ui->progress_bar), 0);
    ocr->image_surface = load_image(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser)));
    if (ocr->image_surface->w >= 500 || ocr->image_surface->h >= 500)
    {
        ocr->image_surface = resize(ocr->image_surface);
        SDL_SaveBMP(ocr->image_surface, "./tmp/resize.bmp");
        gtk_image_set_from_file(GTK_IMAGE(ocr->ui->image), "./tmp/resize.bmp");
    }
    else
    {
        gtk_image_set_from_file(GTK_IMAGE(ocr->ui->image), gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser)));
    }
    ocr->ui->number = find_image(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser)));
    return FALSE;
}

gboolean on_rotation_auto_state_set(GtkSwitch *switch_button, gboolean state, gpointer user_data)
{
    OCR *ocr = user_data;
    ocr->auto_rot = state;
    return FALSE;
}

gboolean on_clear_button_clicked(GtkButton *button, gpointer user_data)
{
    OCR *ocr = user_data;
    ocr->image_surface = NULL;
    gtk_image_set_from_file(GTK_IMAGE(ocr->ui->image), "./images/logo_v2.png");
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(ocr->ui->terminal_window), gtk_text_buffer_new(NULL));
    write_terminal(ocr, "Welcome to Nuke's OCR\n");
    gtk_range_set_value(GTK_RANGE(ocr->ui->rotation_scale), 0);
    gtk_file_chooser_unselect_all(GTK_FILE_CHOOSER(ocr->ui->file_chooser));
    return FALSE;
}

void help()
{
    printf("Welcome to Nuke's OCR\nHere are the commands : \n");
    printf("   --help Give the list of available commands\n");
    printf("   --train  Train the neural network for the OCR\n");
    printf("   (Without arguments)  Launch the GUI\n");
}

gboolean launcher(GtkButton* button, gpointer user_data)
{
    SDL_Surface* croped;
    OCR *ocr = user_data;

    if (ocr->image_surface == NULL)
    {
        write_terminal(ocr, "No image loaded\n");
        return FALSE;
    }
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(ocr->ui->progress_bar), 0.1);
    preprocessing(ocr);
    if (ocr->auto_rot == TRUE)
    {
        float teta = HoughTransform(ocr->image_surface);
        ocr->image_surface = SDL_RotationCentralN(ocr->image_surface, teta);
    }
    else
    {
        ocr->image_surface = SDL_RotationCentralN(ocr->image_surface, gtk_range_get_value(GTK_RANGE(ocr->ui->rotation_scale)));
    }
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(ocr->ui->progress_bar), 0.3);

    croped = crop(ocr->image_surface);
    if(croped)
    {
        ocr->image_surface = colorationCrop(croped);
        lines(ocr->image_surface);
    }
    else
        coloration(ocr->image_surface);
        
    SDL_SaveBMP(ocr->image_surface, "./tmp/lines.bmp");
    save_image(ocr);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(ocr->ui->progress_bar), 0.5);

    main_network(ocr->sudoku, ocr->ui->number);
    
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(ocr->ui->progress_bar), 0.7);

    sudoku(ocr->sudoku);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(ocr->ui->progress_bar), 0.9);

    rebuild_image(ocr);

    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(ocr->ui->progress_bar), 1);
    save_image(ocr);

    ocr->image_surface = NULL; // Reset the loaded image
    return FALSE;
}

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        if (strcmp(argv[1], "--help") == 0)
        {
            help();
        }
        if (strcmp(argv[1], "--train") == 0)
        {
            training();
        }
        return 0;
    }
    else
    {
        // Initializes GTK.
        gtk_init(NULL, NULL);

        // Constructs a GtkBuilder instance.
        GtkBuilder* builder = gtk_builder_new ();

        SDL_Surface* image_surface = NULL;
        SDL_Surface* image_surface_rotated = NULL;

        GError* error = NULL;
        if (gtk_builder_add_from_file (builder, "src/ui/Interface.glade", &error) == 0)
        {
            g_printerr ("Error loading file: %s\n", error->message);
            g_clear_error (&error);
            return 1;
        }
        GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
        GtkButton* clear_button = GTK_BUTTON(gtk_builder_get_object(builder, "header_clear_button"));
        GtkFileChooserButton* file_chooser = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "file_selector"));
        GtkScale* rotation_scale = GTK_SCALE(gtk_builder_get_object(builder, "rotation_scale"));
        GtkSwitch* rotation_auto = GTK_SWITCH(gtk_builder_get_object(builder, "rotation_auto"));
        GtkButton* start_button = GTK_BUTTON(gtk_builder_get_object(builder, "start_button"));
        GtkImage* image = GTK_IMAGE(gtk_builder_get_object(builder, "image_display"));
        GtkProgressBar* progress_bar = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "progress_bar"));
        GtkEntry* output_name = GTK_ENTRY(gtk_builder_get_object(builder, "output_name"));
        GtkTextView* terminal = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "terminal_window"));

        UI ui =
        {
            .window = window,
            .clear_button = clear_button,
            .file_chooser = file_chooser,
            .rotation_scale = rotation_scale,
            .rotation_auto = rotation_auto,
            .start_button = start_button,
            .image = image,
            .progress_bar = progress_bar,
            .output_name = output_name,
            .terminal_window = terminal,
        };

        OCR ocr =
        {
            .image_surface = image_surface,
            .image_surface_rotated = image_surface_rotated,
            .auto_rot = FALSE,
            .ui = &ui,
        };

        init_sudoku(ocr.sudoku);

        gtk_range_set_range(GTK_RANGE(rotation_scale), 0, 360);
        gtk_range_set_show_fill_level(GTK_RANGE(rotation_scale), TRUE);
        gtk_range_set_value(GTK_RANGE(rotation_scale), 0);
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(file_chooser), "./images/");
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), 0);

        g_signal_connect(window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
        g_signal_connect(file_chooser, "file-set", G_CALLBACK (on_file_chooser_file_set), &ocr);
        g_signal_connect(start_button, "clicked", G_CALLBACK (launcher), &ocr);
        g_signal_connect(rotation_auto, "state-set", G_CALLBACK (on_rotation_auto_state_set), &ocr);
        g_signal_connect(rotation_scale, "change-value", G_CALLBACK(on_scale_change), &ocr);
        g_signal_connect(output_name, "changed", G_CALLBACK(on_output_name_changed), &ocr);
        g_signal_connect(clear_button, "clicked", G_CALLBACK(on_clear_button_clicked), &ocr);

        write_terminal(&ocr, "Welcome to Nuke's OCR\n");

        gtk_main();

        SDL_FreeSurface(image_surface);
        SDL_FreeSurface(image_surface_rotated);

        return 0;
    }
}
