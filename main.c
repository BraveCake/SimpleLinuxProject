#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#define boolean char
#define CH_GET_OBJECT( builder, name, type, data ) \
    data->name = type( gtk_builder_get_object( builder, #name ) )
#define CH_GET_WIDGET( builder, name, data ) \
    CH_GET_OBJECT( builder, name, GTK_WIDGET, data )
#define UI_FILE "p5.glade"
#define GW( name ) CH_GET_WIDGET( builder, name, data )
typedef struct user_data //struct contains all the objects of GTK GUI we're doing so because the callback functions can only take 2 arguments maximum we need more to handle the signals
{
    GtkWidget *window,*save_changes,*command_input,*edit_btn,*search_location,*search_btn,*search_result,*choose_file,*new_name,*rename_btn,*pid_list;

}user_data;
  void * edit_man(char * man)
  {
      char * path = "/usr/share/man/man1/"; //default path of man
    char * command = (char * ) malloc(1000);  //allocates memory for the command string
    memset(command,0,1000);
    strcpy(command,"gunzip "); //unzip the man file if exists
    strcat(command,path);
    strcat(command,man);
    strcat(command,".1");
    system(command);
    memset(command,0,1000);
    strcpy(command,"gedit "); //edit the man file through gedit program
    strcat(command,path);
    strcat(command,man);
    strcat(command,".1");
    system(command);
        memset(command,0,1000);
    free(command);
  }

void save_edit(char * man)  // compresses the edited man file by the edit_man function otherwise it will not be recognised by man command
{
    char * path = "/usr/share/man/man1/";
    char * command = (char * ) malloc(1000);
    memset(command,0,1000);
    strcpy(command,"gzip ");
    strcat(command,path);
    strcat(command,man);
    strcat(command,".1");
    system(command);
}
 char  * find_conf(char directory [])
{
    char * buffer =(char *)malloc(10000);
    memset(buffer,0,1000);
    char  cmd[200]="find ";
    strcat(cmd,directory);
    strcat(cmd, " -name *.conf;");
    FILE * result = popen(cmd,"r"); //excute a command and treat the output stream which contains the result as a file

    if( result !=NULL)
    {
       fread(buffer,1000,1,result); //store or read the result inside a buffer

    }
    if (!strlen(buffer)){return "No files were found";} //if the buffer is empty or there's an error return nothing was found
    buffer[strlen(buffer)-1]=0; //make sure the buffer is terminated by a null character
    pclose(result); //close the file that we were reading
    return buffer;
}
boolean illega_char_exists(char* str) // if there's slash inside a string it will return True 1 otherwise false 0
{
        for (int c=0; c<strlen(str); ++c)
    if(str[c]=='/')
    return 1;
return 0;
}
char * get_all_pid ()
{
  char * buffer =(gchar *)malloc(10000);
  char * cmd = "ps -eo pid";
   FILE * result = popen(cmd,"r");
    if( result !=NULL)
    {
        fread(buffer,10000,1,result);

    }
    buffer[strlen(buffer)-1]=0;
    pclose(result);
    return buffer;
}
boolean rename_file(char * path,char *name, char *new_name){
if (illega_char_exists(new_name))
    return 0;
char * command = (char *) malloc(1000);
memset(command,0,1000);
strcpy(command,"mv ");
strcat(command,name);
char * temp = (char *) malloc(1000);
memset(temp,0,1000);
strcpy(temp,path);
strcat(temp,"/");
strcat(temp,new_name);
strcat(command," ");
strcat(command,temp);
system(command);
free(command);
free(temp);
return 1;
}


void command_argument(int argc, char ** argv)
{
        if(argc>1)
    {
        if(!strcmp("1",argv[1]))
        {
            if (argc==2)
                puts(find_conf("./"));
            else
                puts(find_conf(argv[2]));

        }
        if(!strcmp("2",argv[1]))
        {
                 if(argc!=3)
                puts("Please try again and enter the command name that you want to edit its manual page\n");
            else {
                edit_man(argv[2]);
                printf("You have opened %s Manual Page Enter Y to save changes",argv[1]);
                char confirm;
                confirm = getchar();
                if(confirm =='y')
                    save_edit(argv[2]);

            }

        }
         if(!strcmp("3",argv[1]))
         {
  if(argc!=4)
                puts("Please try again and enter the file directory & new name");
else
{
    char path [100]={0};
    int last =strrchr(argv[2],'/')-argv[2];
    for (int i=0; i<=last; ++i)
    {
        path[i]= argv[2][i];
    }
rename_file(path,argv[2],argv[3]);

}
         }
         if (!strcmp("4",argv[1]))
         {
             puts(get_all_pid());
         }
         if(!strcmp("-h",argv[1]))
         {
            //man command here not added yet
         }
exit(0);
    }
}
GtkBuilder * builder;

int main(int argc, char ** argv)
{
    command_argument(argc,argv);
    gchar * pids=get_all_pid();
     user_data* data;
    gtk_init( &argc, &argv );
    char * glade =getenv("glade");
     builder = gtk_builder_new_from_file(glade);
     data=g_slice_new(user_data);
   GW(window);
   GW(command_input);
   GW(edit_btn);
   GW(search_btn);
   GW(search_location);
   GW(search_result);
   GW(choose_file);
   GW(new_name);
   GW(pid_list);
   GW(rename_btn);
   GW(save_changes);
   GW(pid_list);
    GtkTreeStore * treestore = GTK_TREE_STORE(gtk_builder_get_object( builder, "treestore" ));
    GtkTreeViewColumn * pid_c = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object( builder, "pid_c" ));
    GtkCellRenderer * pid_r = GTK_CELL_RENDERER(gtk_builder_get_object( builder, "pid_r" ));
    gtk_tree_view_column_add_attribute(pid_c,pid_r,"text",0);
   GtkTreeIter iter;
    gchar * id = strtok(pids,"\n");
    id = strtok(NULL,"\n");
    while (id!=NULL) {
      gtk_tree_store_append(treestore,&iter,NULL);
   gtk_tree_store_set(treestore,&iter,0,id,-1);
   id = strtok(NULL,"\n");
    }
   g_signal_connect(data->window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
    gtk_builder_connect_signals(builder,data);
    g_object_unref( G_OBJECT( builder ) );
     gtk_widget_show(data->window );
     gtk_main();
     g_slice_free(user_data,data);
}
void edit_btn_clicked_cb(GtkButton*btn,user_data * data)
{
    GtkEntry * command_input = GTK_ENTRY(data->command_input);
    gchar * command = gtk_entry_get_text(command_input);
    edit_man(command);
}
void search_btn_clicked_cb(GtkButton*btn,user_data * data)
{
    get_all_pid();
    gchar * search_path = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(data->search_location));
    if (search_path==NULL)
        return;
    GtkLabel * output = GTK_LABEL(data->search_result);
    gchar * result = find_conf(search_path);
    gtk_label_set_text(output,result   );

}
  void save_changes_clicked_cb(GtkButton*btn,user_data * data)
  {
    GtkEntry * command_input = GTK_ENTRY(data->command_input);
    gchar * man = gtk_entry_get_text(command_input);
    save_edit(man);
  }
void rename_btn_clicked_cb(GtkButton*btn,user_data * data)
{
     gchar * name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(data->choose_file));
     gchar * path = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(data->choose_file));
     gchar * new_name = gtk_entry_get_text(GTK_ENTRY(data->new_name));
     char* txt;
     boolean modified = rename_file(path,name,new_name);
     if(!modified)txt="Couldn't modify the file name because it contains invalid character(s)";
     else txt = "Renamed Succssfully";
     GtkWidget *dialog ;
    dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, txt );
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy( GTK_WIDGET(dialog) );
    if(modified)
    gtk_file_chooser_unselect_all(GTK_FILE_CHOOSER(data->choose_file));


}
