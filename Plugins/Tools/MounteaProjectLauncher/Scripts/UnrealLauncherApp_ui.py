import tkinter as tk
from tkinter import messagebox, filedialog, StringVar

from .utility import find_umap_files, detect_unreal_versions, find_unreal_project, get_absolute_path
from .ui_elements import *


class UnrealLauncherAppUI:
    def __init__(self, app, project_directory, has_uproject_file, config):
        self.app = app
        self.project_directory = project_directory
        self.has_uproject_file = has_uproject_file
        self.config = config
        self.root = tk.Tk()
        self.root.title("Mountea Project Launcher")
        self.root.geometry("1600x600")
        path_to_icon = get_absolute_path(os.path.dirname(os.path.realpath(__file__)), "MPLIcon.ico")
        self.root.iconbitmap(path_to_icon)
        self.root.config(bg="dimgray")
        
        self.launch_options = tk.StringVar()
        self.launch_options.trace_add("write", self.update_launch_option)
        self.selected_version = tk.StringVar()
        self.selected_version.trace_add("write", self.update_engine_version)
        
        self.tooltip = None
        self.selected_index = None
        self.tooltip_after_id = None
        
        self.command_display_var = tk.StringVar()
        self.command_display_var.set(self.app.command)
        
        # Bind a callback to handle window closure
        self.root.protocol("WM_DELETE_WINDOW", self.on_close)
        
        self.initialize_ui()
        
        
    def on_close(self):
        # Remove the trace for launch_options
        #self.launch_options.trace_remove("write", 'update_launch_option')
        
        # Remove the trace for selected_version
        #self.selected_version.trace_remove("write", 'update_engine_version')
        
        # This method is called when the window is closed
        # Destroy all widgets
        print("Closing the window...")
        for widget in self.root.winfo_children():
            widget.destroy()
        # Perform any other cleanup tasks
        
        # Close the window
        self.root.destroy()

        
    def on_select(self, event=None):
        # Update the selected index when an item is clicked
        selection = self.maps_listbox.curselection()
        if selection:
            self.selected_index = selection[0]
        else:  # No selection, e.g., when clicking outside the list items
            self.selected_index = None
            
        self.app.set_selected_map(self.app.maps_with_paths[self.maps_listbox.get(self.selected_index)])

        self.update_command_display()


    def initialize_ui(self):            
        if not self.project_directory or not self.has_uproject_file:
            self.setup_selection_page()
        else:
            self.setup_main_page()
            
        self.setup_version_label()
        
        self.root.mainloop()
        
        
    def destroy_widgets(self):
        try:
            # Check if the root window exists
            if self.root.winfo_exists():
                # Destroy all widgets in the root window
                for widget in self.root.winfo_children():
                    widget.destroy()
        except tk.TclError:
            # Handle the TclError gracefully (e.g., log the error or ignore it)
            pass


    def setup_version_label(self):
        version_text = self.config.get("version")
        self.version_label = tk.Label(self.root, text=f"Version: {version_text}", bg="dimgray")
        self.version_label.pack(side=tk.BOTTOM, fill=tk.X)


    def setup_selection_page(self):
        self.destroy_widgets()
            
        select_project_folder_label = tk.Label(self.root, text="SELECT PROJECT", font=('Helvetica', 14, 'bold'), bg="dimgray")
        select_project_folder_label.pack(fill=tk.X, expand=False)

        select_project_folder_button = create_button(self.root, "Select Project Folder", self.select_project_folder)
        select_project_folder_button.pack(pady=10)


    def setup_main_page(self):
        self.destroy_widgets()
            
        main_paned_window = tk.PanedWindow(self.root, orient=tk.VERTICAL, bg="dimgray")
        main_paned_window.pack(fill=tk.BOTH, expand=True, pady=10, padx=10)

        self.upper_paned_window = tk.PanedWindow(main_paned_window, orient=tk.HORIZONTAL, bg="dimgray")
        self.upper_paned_window.pack(padx=10, pady=10)
        main_paned_window.add(self.upper_paned_window, stretch='always')

        maps_frame = tk.Frame(self.upper_paned_window, borderwidth=2, relief=tk.SOLID)   
        maps_frame.pack(padx=10, pady=10)
        self.upper_paned_window.add(maps_frame, stretch='always')

        maps_label = tk.Label(maps_frame, text="MAPS", font=bold_font)
        maps_label.pack(fill=tk.X, expand=False)

        self.load_button = create_button(maps_frame, "Load Maps", command=self.load_maps) #, image=self.refreshIcon, compound="left", padx=10)
        self.load_button.pack(pady=10)

        self.maps_listbox = tk.Listbox(maps_frame, width=50, height=10)
        scrollbar = tk.Scrollbar(maps_frame, orient="vertical", command=self.maps_listbox.yview)
        self.maps_listbox.configure(yscrollcommand=scrollbar.set)
        self.maps_listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)
        self.maps_listbox.bind("<<ListboxSelect>>", self.on_select)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.right_side_frame = tk.Frame(self.upper_paned_window, borderwidth=2, relief=tk.SOLID)  
        self.upper_paned_window.add(self.right_side_frame, stretch='always')
        
        launch_modes_frame = tk.Frame(self.right_side_frame)  
        launch_modes_label = tk.Label(self.right_side_frame, text="LAUNCH MODES", font=bold_font)
        launch_modes_label.pack(fill=tk.BOTH, expand=False)
        launch_modes_frame.pack(fill=tk.BOTH, expand=True)
        self.launch_modes = self.config.get("launch_commands", {})
        
        first_button_created = False
        for mode in self.launch_modes:
            mode_button = tk.Radiobutton(
                launch_modes_frame, 
                text=mode, 
                variable=self.launch_options,
                #command=self.update_launch_option, 
                value=mode
            )
            mode_button.pack(anchor=tk.W)
            if not first_button_created:
                self.launch_options.set(mode)
                first_button_created = True

        engine_versions_frame = tk.Frame(self.right_side_frame)
        engine_versions_frame.pack(fill=tk.BOTH, expand=True)
        engine_versions_label = tk.Label(engine_versions_frame, text="ENGINE VERSIONS", font=bold_font)
        engine_versions_label.pack(fill=tk.BOTH, expand=False)
        
        first_version_button_created = False
        self.engine_versions = detect_unreal_versions()
        
        for engine_version in self.engine_versions:
            version_button = tk.Radiobutton(
                engine_versions_frame,
                text=f"UE {engine_version}",
                variable=self.selected_version,
                #command=self.update_engine_version,
                value=engine_version
            )
            version_button.pack(anchor=tk.W)
            if not first_version_button_created:
                self.selected_version.set(engine_version)
                first_version_button_created = True

        bottom_frame = tk.Frame(main_paned_window, borderwidth=2, relief=tk.SOLID)  
        main_paned_window.add(bottom_frame, stretch='never')
        
        self.command_display_var = tk.StringVar()
        self.command_display_label = tk.Label(bottom_frame, textvariable=self.command_display_var, bg="white", anchor="w", relief="sunken")
        self.command_display_label.pack(fill=tk.X, padx=5, pady=5)

        self.copy_command_button = create_button(bottom_frame, "Copy Command", command=self.copy_text_to_clipboard)
        self.copy_command_button.pack(side=tk.LEFT, padx=5)

        # New bottom frame for the Launch button
        launch_button_frame = tk.Frame(bottom_frame)  
        launch_button_frame.pack(side=tk.BOTTOM, fill=tk.X)
        # Configure the Launch button to fill the X-axis in its frame
        self.launch_button = create_button(launch_button_frame, "Launch", command=self.execute_command, inputState='disabled')
        self.launch_button.pack(fill=tk.X, padx=10, pady=10)          
        
        version_label = tk.Label(bottom_frame, text=self.config.get("version"), font=bold_font) 
        
        
    def copy_text_to_clipboard(self, event=None):
        """Copy the command display label's text to the clipboard."""
        command_text = self.command_display_var.get()
        self.root.clipboard_clear()
        self.root.clipboard_append(command_text)
        messagebox.showinfo("Info", "Command copied to clipboard")
                

    def setup_maps_frame(self, parent_frame):
        maps_frame = tk.Frame(parent_frame, borderwidth=2, relief=tk.SOLID)
        maps_frame.pack(padx=10, pady=10)
        parent_frame.add(maps_frame, stretch='always')

        maps_label = tk.Label(maps_frame, text="MAPS", font=('Helvetica', 14, 'bold'))
        maps_label.pack(fill=tk.X, expand=False)

        load_button = create_button(maps_frame, "Load Maps", self.load_maps)
        load_button.pack(pady=10)

        self.maps_listbox = tk.Listbox(maps_frame, width=50, height=10)
        scrollbar = tk.Scrollbar(maps_frame, orient="vertical", command=self.maps_listbox.yview)
        self.maps_listbox.configure(yscrollcommand=scrollbar.set)
        self.maps_listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        

    def setup_right_side_frame(self, parent_frame):
        right_side_frame = tk.Frame(parent_frame, borderwidth=2, relief=tk.SOLID)
        parent_frame.add(right_side_frame, stretch='always')

        launch_modes_label = tk.Label(right_side_frame, text="LAUNCH MODES", font=('Helvetica', 14, 'bold'))
        launch_modes_label.pack(fill=tk.BOTH, expand=False)

        launch_modes = self.config.get("launch_commands", {}).keys()
        for mode in launch_modes:
            tk.Radiobutton(right_side_frame, text=mode, variable=tk.StringVar(value="Standalone"), value=mode,
                           command=self.update_command_display).pack(anchor=tk.W)
            
            
    def update_launch_option(self, arg1, arg2, arg3):
        selected_option = self.launch_options.get()
        
        if (selected_option):
            self.app.set_selected_launch(selected_option)
            
            self.update_command_display()        
            
            
    def update_engine_version(self, arg1, arg2, arg3):
        selected_version = self.selected_version.get()
        version_data = self.engine_versions.get(selected_version)
        
        if version_data:
            self.app.set_selected_version(version_data[0])
            
            self.update_command_display()
            
                        
    def enable_launch(self):
        if hasattr(self, 'launch_button'):
            if self.app.command:
                self.launch_button.config(state='normal', borderwidth=2, )
            else:
                self.launch_button.config(state='disabled')
                
                
    def execute_command(self):
        self.app.launch_project()
            
    
    def update_command_display(self):
        """Update the command display based on the current selections."""
        if self.app.command:            
            self.app.update_command()
            command = self.app.command            
            if command:
                self.command_display_var.set(command)                
            else:
                self.command_display_var.set("Unable to construct command.")
        else:
            self.command_display_var.set("")
        self.enable_launch()


    def select_project_folder(self):
        folder_path = filedialog.askdirectory(title="Select Unreal Project Folder")
        if folder_path and find_umap_files(folder_path):
            self.project_directory = folder_path
            self.has_uproject_file = True
            self.app.set_project_directory(folder_path)
            self.app.set_selected_project_file(find_unreal_project(folder_path))
            
            self.initialize_ui()
        else:
            messagebox.showerror("Error", "No .uproject file found in the selected directory.")


    def load_maps(self):
        self.app.maps_with_paths = find_umap_files(self.app.project_directory)
        self.maps_listbox.delete(0, tk.END)
        for friendly_name in self.app.maps_with_paths:
            self.maps_listbox.insert(tk.END, friendly_name)
