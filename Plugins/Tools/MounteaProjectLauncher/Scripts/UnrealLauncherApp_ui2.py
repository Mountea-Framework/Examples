import sys
import os

from PyQt5 import QtCore
from PyQt5.QtWidgets import (QApplication, QWidget, QVBoxLayout, QLabel, QPushButton, QComboBox,
                             QHBoxLayout, QLineEdit, QFileDialog, QListWidget, QSizePolicy, QMessageBox)
from PyQt5.QtCore import Qt
from PyQt5.QtGui import (QFont, QIcon, QFontDatabase)

from .utility import (find_umap_files, find_unreal_project, detect_unreal_versions)

script_dir = os.path.dirname(os.path.realpath(__file__))

icon_filename = "MPLIcon.png"
chevron_right_filename = "icon_chevron_right.png"
chevron_down_filename = "icon_chevron_down.png"
chevron_right_white_filename = "icon_chevron_right_white.png"
chevron_down_white_filename = "icon_chevron_down_white.png"

font_filename = "Inter-VariableFont.ttf"


def get_custom_font():
    custom_font = QFont()
    font_path = os.path.join(script_dir, "..", "Fonts", font_filename).replace("\\", "/")
    font_id = QFontDatabase.addApplicationFont(font_path)
    if font_id != -1:
        font_family = QFontDatabase.applicationFontFamilies(font_id)[0]
        custom_font.setFamily(font_family)
        return custom_font
    else:
        font_family = "Roboto"
        custom_font.setFamily(font_family)
        return custom_font


def get_custom_font_family():
    return get_custom_font().family()


def get_map_list_style():
    maps_list_style = """
        QListWidget {{
            border: none;
            background-color: white;
        }}
        QListWidget::item {{
            background-color: transparent;
            border: none;
            padding: 5px;
            outline: none;
            font-family: {};
            font-size: 14px;
        }}
        QListWidget::item:selected {{
            background-color: #3651ea;
            color: white;
            border: none;
            outline: none;        
        }}
        QListWidget::item:hover {{
            background-color: gray;
            color: black;
            outline: none;
        }}
        QScrollBar:vertical {{
            background: #f0f0f0;
            border: none;
            width: 12px;
            margin: 0px 0px 0px 0px;
        }}
        QScrollBar::handle:vertical {{
            background: #a3a3a3;
            min-height: 30ps;
        }}
        QScrollBar::add-line:vertical {{
            background: none;
            height: 0px;
        }}
        QScrollBar::sub-line:vertical {{
            background: none;
            height: 0px;
        }}
    """.format(get_custom_font_family())
    return maps_list_style


def get_primary_button_style():
    primary_button_style = """
        QPushButton {{
            color: white;
            background-color: #3651ea;
            border: none;
            font-family: {};
            font-size: 14px;
            font-weight: bold;
        }}

        QPushButton:hover {{
            background-color: blue;        
        }}

        QPushButton:disabled {{
            background-color: #b8b9bf;
            color: gray;
        }}
    """.format(get_custom_font_family())
    return primary_button_style


def get_secondary_button_style():
    secondary_button_style = """
            QPushButton {{
                color: #3651ea;
                border: none;
                font-family: {};
                font-size: 12px;
                text-align: right;
                font-weight: bold;
                background: white;
            }}

            QPushButton:hover {{
                background: #b8b9bf;
            }}
    """.format(get_custom_font_family())
    return secondary_button_style


def get_tertiary_button_style():
    tertiary_button_style = """
        QPushButton {{
            color: #3651ea;
            border: none;
            font-family: {};
            font-size: 12px;
            text-align: right;
            font-weight: bold;
        }}

        QPushButton:hover {{
            color: blue;
        }}

        QPushButton:disabled {{
            color: #b8b9bf;
        }}
    """.format(get_custom_font_family())
    return tertiary_button_style


def get_combo_style(arrow_url, arrow_url_active):
    combo_style = """
                QComboBox {{
                    background: white;
                    color: black;
                    padding: 0px 10px 0px 10px;
                    border: none;
                    font-family: {};
                    font-size: 14px;
                }}
                
                QComboBox:on {{
                    background-color: #7c8ce9;
                    color: white;
                }}
                
                QComboBox:hover {{
                    background-color: gray;
                }}

                QComboBox::drop-down {{
                    border: none;
                }}

                QComboBox::down-arrow {{
                    image: url({});
                    width: 14px;
                    height: 14px;
                    padding: 0px 10px 0px 0px;
                }}
                
                QComboBox::down-arrow::on {{
                    image: url({});
                }}
                
                QComboBox::item:hover {{
                    background-color: gray;
                }}
            """.format(get_custom_font_family(), arrow_url, arrow_url_active)
    return combo_style


def get_spacer(horizontal, vertical):
    spacer = QWidget()
    spacer.setFixedHeight(vertical)
    spacer.setFixedWidth(horizontal)
    return spacer


def get_helper_label_style():
    label_style = """
        QLabel {{
            color: black;
            font-family: {};
            font-size: 14px;
            font-align: center;
            border: none;
            background-color: transparent;
            margin: 0px;
        }}
    """.format(get_custom_font_family())
    return label_style


def get_readonly_text_style():
    label_style = """
        QLineEdit {{
            color: gray;
            font-family: {};
            font-size: 12px;
            text-align: left;
            border: none;
            background-color: white;
            margin: 0px;
        }}
    """.format(get_custom_font_family())
    return label_style


def get_app_icon():
    return os.path.join(script_dir, "..", "Icons", icon_filename).replace("\\", "/")


def get_chevron_icon_dark():
    chevron_right_path = os.path.join(script_dir, "..", "Icons", chevron_right_filename).replace("\\", "/")
    chevron_down_path = os.path.join(script_dir, "..", "Icons", chevron_down_filename).replace("\\", "/")
    return chevron_right_path, chevron_down_path


def get_chevron_icon_light():
    chevron_right_white_path = os.path.join(script_dir, "..", "Icons", chevron_right_white_filename).replace("\\", "/")
    chevron_down_white_path = os.path.join(script_dir, "..", "Icons", chevron_down_white_filename).replace("\\", "/")
    return chevron_right_white_path, chevron_down_white_path


class Launcher(QWidget):
    def __init__(self, app):
        super().__init__()

        self.app = app

        self.unreal_versions_map = {}
        self.launch_modes_map = {}

        self.setup_window()

        layout = QVBoxLayout()

        self.setWindowFlags(Qt.WindowCloseButtonHint)

        header = QWidget()
        header_layout = QHBoxLayout(header)
        header_layout.setContentsMargins(0, 0, 0, 0)
        header_layout.setAlignment(Qt.AlignAbsolute | Qt.AlignTop)

        title_wrapper, title_layout = self.create_title_widget()
        title_label = self.create_title_label()
        spacer = get_spacer(0, 10)
        self.project_name_btn, self.left_label = self.create_project_button()

        title_layout.addWidget(title_label)
        title_layout.addWidget(spacer)
        title_layout.addWidget(self.project_name_btn)

        header_layout.addWidget(title_wrapper)

        layout.addWidget(header)

        self.wrapper = QWidget()
        self.wrapper_layout = QVBoxLayout(self.wrapper)
        self.wrapper.setVisible(False)
        layout.addWidget(self.wrapper)

        divider = QLabel()
        divider.setFrameShape(QLabel.HLine)
        divider.setFrameShadow(QLabel.Sunken)
        divider.setStyleSheet("color: #dcdde1;")
        self.wrapper_layout.addWidget(divider)

        maps_title_layout = QHBoxLayout()
        maps_title = QLabel("Maps")
        maps_title.setFont(QFont(get_custom_font_family(), 12, QFont.Bold))
        self.reload_maps_btn = QPushButton("Reload maps")
        self.reload_maps_btn.setStyleSheet(get_tertiary_button_style())
        self.reload_maps_btn.clicked.connect(self.load_maps)
        self.reload_maps_btn.setToolTip(
            "Reloads all Maps within selected Project Folder\n"
            "Resets selection and thus resetting command")
        maps_title_layout.addWidget(maps_title)
        maps_title_layout.addWidget(self.reload_maps_btn)
        self.wrapper_layout.addLayout(maps_title_layout)

        self.wrapper_layout.addWidget(get_spacer(0, 2))

        self.maps_list = QListWidget()
        self.maps_list.setStyleSheet(get_map_list_style())
        self.maps_list.itemSelectionChanged.connect(self.handle_selection_change)
        self.wrapper_layout.addWidget(self.maps_list)

        self.wrapper_layout.addWidget(get_spacer(0, 2))

        self.launch_modes = self.app.config.get("launch_commands", {})
        self.launch_mode_combo = QComboBox()
        for launch_mode in self.launch_modes:
            key = f"Launch Mode: {launch_mode}"
            self.launch_modes_map[key] = launch_mode
        self.launch_mode_combo.addItems(self.launch_modes_map)
        self.launch_mode_combo.setStyleSheet(get_combo_style(get_chevron_icon_dark()[1], get_chevron_icon_light()[1]))
        self.launch_mode_combo.setFixedHeight(50)
        self.launch_mode_combo.currentIndexChanged.connect(self.launch_mode_changed)

        self.unreal_combo = QComboBox()
        self.unreal_versions = detect_unreal_versions()
        for version in self.unreal_versions:
            key = f"Unreal Engine: {version}"
            self.unreal_versions_map[key] = version
        self.unreal_combo.addItems(self.unreal_versions_map)
        self.unreal_combo.setStyleSheet(get_combo_style(get_chevron_icon_dark()[1], get_chevron_icon_light()[1]))
        self.unreal_combo.setFixedHeight(50)
        self.unreal_combo.currentIndexChanged.connect(self.engine_version_changed)

        combo_layout = QHBoxLayout()
        combo_layout.addWidget(self.launch_mode_combo)
        combo_layout.addWidget(get_spacer(2, 0))
        combo_layout.addWidget(self.unreal_combo)
        self.wrapper_layout.addLayout(combo_layout)

        self.wrapper_layout.addWidget(get_spacer(0, 2))

        path_layout = QHBoxLayout()
        self.path_edit = QLineEdit("")
        self.path_edit.setReadOnly(True)
        path_layout.addWidget(self.path_edit)
        self.path_edit.setStyleSheet(get_readonly_text_style())
        self.path_edit.setFixedHeight(50)

        copy_button = QPushButton("Copy")
        copy_button.setStyleSheet(get_tertiary_button_style())
        copy_button.clicked.connect(self.copy_command)
        copy_button.setToolTip(
            "Will copy generated command to clipboard\n"
            "Keep in mind that this will replace last active item in your clipboard")
        path_layout.addWidget(copy_button)

        self.wrapper_layout.addLayout(path_layout)

        self.wrapper_layout.addWidget(get_spacer(0, 2))

        self.launch_project_btn = QPushButton("Launch project")
        self.launch_project_btn.setStyleSheet(get_primary_button_style())
        self.launch_project_btn.setDisabled(True)
        self.launch_project_btn.setToolTip(
            "To enable Launch Project you need to select Map, Launch Mode and Engine Version")

        self.launch_project_btn.setFixedHeight(50)
        self.launch_project_btn.clicked.connect(self.launch_project)
        self.wrapper_layout.addWidget(self.launch_project_btn)

        self.setLayout(layout)

    def setup_window(self):
        self.setStyleSheet("background: #eaebef;")
        self.setWindowFlags(Qt.WindowCloseButtonHint)
        self.setWindowIcon(QIcon(get_app_icon()))
        self.setGeometry(100, 100, 550, 700)

    def create_title_widget(self):
        title_wrapper = QWidget()
        title_layout = QVBoxLayout(title_wrapper)
        title_layout.setAlignment(Qt.AlignTop)
        return title_wrapper, title_layout

    def create_title_label(self):
        title = QLabel("Mountea Project Launcher")
        title.setAlignment(Qt.AlignLeft)
        title.setFont(QFont(get_custom_font_family(), 16, QFont.Bold))
        return title

    def create_project_button(self):
        project_button = QPushButton()
        button_layout = QHBoxLayout()
        project_button.setLayout(button_layout)

        left_label = QLabel("Open Project Folder")
        left_label.setStyleSheet(get_helper_label_style())
        button_layout.addWidget(left_label, alignment=Qt.AlignLeft)

        button_layout.addStretch()

        icon_label = QLabel()
        icon_label.setPixmap(QIcon(get_chevron_icon_dark()[0]).pixmap(16, 16))
        icon_label.setStyleSheet("background: transparent;")
        button_layout.addWidget(icon_label, alignment=Qt.AlignRight)

        project_button.setStyleSheet(get_secondary_button_style())
        project_button.setFixedHeight(50)
        project_button.setSizePolicy(QSizePolicy.Minimum, QSizePolicy.Fixed)
        project_button.setToolTip("Open Project folder (must be Unreal Engine project folder)")
        project_button.clicked.connect(self.open_file_dialogue)
        project_button.installEventFilter(self)

        return project_button, left_label

    def toggle_night_mode(self):
        print(f"toggling")

    def eventFilter(self, obj, event):
        if obj == self.project_name_btn:
            if event.type() == QtCore.QEvent.Enter:
                if self.app.project_directory:
                    self.left_label.setText("Open Different Project Folder")
                else:
                    self.left_label.setText("Open Project Folder")
            elif event.type() == QtCore.QEvent.Leave:
                if self.app.project_directory:
                    folder_name = os.path.basename(self.app.project_directory)
                    self.left_label.setText(f"{folder_name} Folder")
                else:
                    self.left_label.setText("Open Project Folder")

        return super().eventFilter(obj, event)

    def open_file_dialogue(self):
        options = QFileDialog.Options()
        options |= QFileDialog.DontUseNativeDialog
        folder = QFileDialog.getExistingDirectory(self, "Select Folder")
        if folder:
            if find_umap_files(folder):
                self.app.reset_selection()
                # Save the folder path
                self.folder_path = folder
                # Show the wrapper
                self.wrapper.setVisible(True)
                self.setGeometry(100, 100, 550, 700)

                self.app.set_project_directory(folder)
                self.app.set_selected_project_file(find_unreal_project(folder))

                folder_name = os.path.basename(folder)
                self.left_label.setText(f"{folder_name} Folder")
                self.project_name_btn.setToolTip(
                    f"Currently opened project is {folder_name} | You can select any other by selecting different folder")

                self.launch_mode_changed()
                self.engine_version_changed()
                self.load_maps()
            else:
                QMessageBox.critical(self, "Error", "Invalid folder path selected.")
        else:
            QMessageBox.information(self, "Info", "Folder selection canceled.")

    def copy_command(self):
        if self.app.command:
            clipboard = QApplication.clipboard()
            clipboard.clear()
            clipboard.setText(self.app.command)
            QMessageBox.information(self, "Info", "Command copied to clipboard")
        else:
            QMessageBox.information(self, "Info", "Command is not yet valid")

    def load_maps(self):
        self.app.maps_with_paths = find_umap_files(self.app.project_directory)
        self.maps_list.clear()
        for friendly_name in self.app.maps_with_paths:
            self.maps_list.addItem(friendly_name)
        self.app.set_selected_map("")
        self.update_ui()

    def launch_mode_changed(self):
        key = self.launch_mode_combo.currentText()
        selected_mode = self.launch_modes_map[key]
        self.app.set_selected_launch(selected_mode)
        self.update_ui()

    def engine_version_changed(self):
        selected_engine = self.unreal_combo.currentText()
        selected_version = self.unreal_versions_map[selected_engine]
        selected_version = self.unreal_versions.get(selected_version)[0]
        self.app.set_selected_version(selected_version)
        self.update_ui()

    def handle_selection_change(self):
        selected_items = self.maps_list.selectedItems()
        if selected_items:
            selected_item = selected_items[0]
            selected_map_path = self.app.maps_with_paths[selected_item.text()]
            self.app.set_selected_map(selected_map_path)
            self.update_ui()

    def update_ui(self):
        if self.app.command:
            self.path_edit.setText(self.app.command)
        else:
            self.path_edit.setText("")
        self.enable_launch_button()

    def enable_launch_button(self):
        if self.app.command:
            self.launch_project_btn.setDisabled(False)
            self.launch_project_btn.setToolTip(
                "Will execute command and if your selection is valid, then it will launch a project")
        else:
            self.launch_project_btn.setDisabled(True)
            self.launch_project_btn.setToolTip(
                "To enable Launch Project you need to select Map, Launch Mode and Engine Version")

    def launch_project(self):
        if self.app.command:
            self.app.launch_project()


class LauncherApp(QApplication):
    def __init__(self, parent, argv):
        super().__init__(argv)
        self.parent = parent
        self.launcher = Launcher(self.parent)

    def start(self):
        self.launcher.setWindowTitle("Mountea Project Launcher")
        self.launcher.show()
        sys.exit(self.exec_())
