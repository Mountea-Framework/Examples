import subprocess
import json
import os
from pathlib import Path


from .utility import read_config, CONFIG_FILE


def construct_command(selected_map, selected_mode, uproject_file, project_directory, unreal_versions_info):
    if not selected_map or not selected_mode or not uproject_file or not project_directory or not unreal_versions_info:
        print("Missing required parameters to construct the command.")
        return ""

    # Retrieve the full path to the selected Unreal Engine version's editor executable
    editor_executable = unreal_versions_info

    # Check if we have a launch command format for the selected mode
    launch_command_format = read_config().get("launch_commands", {}).get(selected_mode)
    if not launch_command_format:
        print(f"Error: Launch command format for {selected_mode} not found in the config.")
        return ""

    # Format the command with the necessary paths
    uproject_path = Path(project_directory) / uproject_file
    map_path = f'"{selected_map}"' if selected_map else ""

    # Note: The `map_path` variable must be enclosed in double quotes if it is not empty,
    # to ensure that the command works correctly with paths that contain spaces.

    command = launch_command_format.format(
        executable=editor_executable,
        uproject_path=uproject_path,
        map_path=map_path
    )
        
    return command


def update_launch_option(app_instance, mode):
    """Updates the launch option."""
    app_instance.launch_options.set(mode)
    app_instance.update_command_display()
    app_instance.enable_launch()
    

def execute_command(command):
    try:
        # The actual execution of the command can be separated into its own function for clarity
        subprocess.Popen(command, shell=True)
        print(f"Launched with command: {command}")
    except Exception as e:
        print(f"Failed to execute the command: {e}")

