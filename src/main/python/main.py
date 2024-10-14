import os
import sys
import threading
from datetime import datetime
from time import sleep
from typing import List

import matplotlib.pyplot as plt
import matplotlib.axes as mtAxes
import numpy as np
import soundfile as sf
import pickle
from dependencies.constantQ.main import constantq
from dependencies.formant_CGDZP.main import formant_CGDZP
from dependencies.gammatonegram.main import gammatonegram
from dependencies.pitch_srh.main import pitch_srh
from dependencies.resample.main import resample
from dependencies.SINGLE_FREQ_FILTER_FS.main import \
    SINGLE_FREQ_FILTER_FS as single_freq_filter_fs
from dependencies.voiced_unvoiced_own.main import voiced_unvoiced_own
from dependencies.ZERO_TIME_WIND_SPECTRUM.main import zero_time_wind_spectrum

from panes.factory import Pane_Factory
from panes.base import Pane_Base

from components.draggable_box import DraggableBox
import utils

from matplotlib.backends.backend_qt5agg import \
    FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import \
    NavigationToolbar2QT as NavigationToolbar
from ppg_runtime.application_context import PPGStore
from ppg_runtime.application_context.PyQt5 import (ApplicationContext,
                                                   PPGLifeCycle)
from PyQt5.QtCore import Qt, QTimer
from PyQt5.QtGui import QPixmap
from PyQt5.QtWidgets import (QAction, QActionGroup, QApplication, QFileDialog,
                             QGroupBox, QHBoxLayout, QLabel, QMainWindow,
                             QMenu, QMessageBox, QRadioButton, QSplitter,
                             QTextEdit, QToolButton, QVBoxLayout, QWidget,
                             QLineEdit, QDialog, QPushButton)
from scipy.signal import spectrogram

from version import meta_info

def show_error_message(message):
    msg = QMessageBox()
    msg.setIcon(QMessageBox.Critical)
    msg.setText(message)
    msg.setWindowTitle("Error")
    msg.exec_()

def get_file_extension(file_name):
    # Use os.path.splitext to split the file name into its root and extension
    print(file_name)
    _, extension = os.path.splitext(file_name)
    # Remove the leading '.' from the extension
    return extension[1:]

def has_second_channel(audio):
    print(audio.ndim, audio.shape)
    if audio.ndim == 1:
        return False
    elif audio.ndim == 2 and audio.shape[1] == 2:
        # Type 2 audio (shape: (n, 2))
        # Take only the first channel
        return True
    
def process_audio(audio):
    if audio.ndim == 1:
        # Type 1 audio (shape: (n,))
        return audio
    elif audio.ndim == 2 and audio.shape[1] == 2:
        # Type 2 audio (shape: (n, 2))
        # Take only the first channel
        return audio[:, 0]
    else:
        # Invalid audio format
        raise ValueError("Invalid audio format")
class AboutInfoWindow(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("About This App")

        # Create layout
        layout = QVBoxLayout()

        # Version number
        version_label = QLabel(f"Version: {meta_info['version']}")
        version_label.setAlignment(Qt.AlignCenter)
        layout.addWidget(version_label)

        # Logos
        logo1 = QLabel()
        pixmap1 = QPixmap("../icons/iiith.jpeg")  # Update with actual path
        logo1.setPixmap(pixmap1)
        logo1.setAlignment(Qt.AlignCenter)
        
        logo2 = QLabel()
        pixmap2 = QPixmap("path/to/logo2.png")  # Update with actual path
        logo2.setPixmap(pixmap2)
        logo2.setAlignment(Qt.AlignCenter)

        logo_layout = QHBoxLayout()
        logo_layout.addWidget(logo1)
        logo_layout.addWidget(logo2)
        layout.addLayout(logo_layout)

        # Trademark/Ownership mark
        trademark_label = QLabel("© Speech Processing Lab (SPL), IIITH. All rights reserved.")
        trademark_label.setAlignment(Qt.AlignCenter)
        layout.addWidget(trademark_label)

        # Git repository link
        repo_label = QLabel('<a href="https://github.com/Abhinavreddy-B/Waveform-Wizard-2">GitHub Repository</a>')
        repo_label.setOpenExternalLinks(True)
        repo_label.setAlignment(Qt.AlignCenter)
        layout.addWidget(repo_label)

        # Close button
        close_button = QPushButton("Close")
        close_button.clicked.connect(self.accept)
        layout.addWidget(close_button)

        # Set layout for the dialog
        self.setLayout(layout)


class AudioComponent(QGroupBox):
    def __init__(self, file_name):
        super().__init__(file_name)
        self.initUI()
        self.file_name = file_name
        self.data = None
        self.fs = None
        self.resampled_data = None
        self.resampled_fs = None

        # Variable holding data of second channel. (for EGG)
        self.second_data = None
        self.second_fs = None
        self.second_channel_available = False

    def initUI(self):
        self.layout_area = QVBoxLayout()
        self.setLayout(self.layout_area)

    def update_in_background(self, func, callback=None):
        def wrapped_worker():
            func()
            if callback:
                callback()

        def background_func():
            thread = threading.Thread(target=wrapped_worker)
            thread.start()
        return background_func
    
    def add_waveform_plot_area(self):
        self.plot_waveform = plt.figure(facecolor='none')
        self.ax_waveform = self.plot_waveform.add_subplot(111, facecolor='none')
        self.ax_waveform.set_facecolor('None')
        self.plot_waveform.tight_layout()
        self.canvas_waveform = FigureCanvas(self.plot_waveform)
        self.canvas_waveform.setStyleSheet("background: transparent;")

        self.draggable_box = DraggableBox(self.ax_waveform, self.__update_plot_x_lims, self._time)
        self.canvas_waveform.mpl_connect('button_press_event', self.draggable_box.on_press)
        self.canvas_waveform.mpl_connect('button_release_event', self.draggable_box.on_release)
        self.canvas_waveform.mpl_connect('motion_notify_event', self.draggable_box.on_motion)

        self.action_button_layout_waveform = QHBoxLayout()
        self.zoom_in_action = QAction('Zoom  +')
        self.zoom_in_action.triggered.connect(self.zoom_in)
        self.tool_button_zoom_in = QToolButton(self)
        self.tool_button_zoom_in.setDefaultAction(self.zoom_in_action)
        self.zoom_out_action = QAction('Zoom -')
        self.zoom_out_action.triggered.connect(self.zoom_out)
        self.tool_button_zoom_out = QToolButton(self)
        self.tool_button_zoom_out.setDefaultAction(self.zoom_out_action)
        self.action_button_layout_waveform.addWidget(self.tool_button_zoom_in)
        self.action_button_layout_waveform.addWidget(self.tool_button_zoom_out)
        self.action_button_layout_waveform.addStretch()
        
        self.layout_area.addLayout(self.action_button_layout_waveform)
        self.layout_area.addWidget(self.canvas_waveform)

    def update_plot(self):
        # self.set_loading_screen_in_plot()
        # self.ax_waveform.clear()
        time = len(self.data)/self.fs

        x = np.linspace(0, time, len(self.data))
        self.ax_waveform.plot(x, self.data)
        self.canvas_waveform.draw()

    def set_data(self, data, fs):
        print(data.shape)
        self.data = data
        self.fs = fs

        self.resampled_data = resample(self.data, 4000, self.fs)
        self.resampled_fs = 4000
        
        self._time = len(self.data)/self.fs
        
        self.add_waveform_plot_area()
        self.update_plot()


    def _add_pane(self, pane_name):
        pane_class = Pane_Factory.get_pane_class_by_name(pane_name)
        x_left, x_right = self.draggable_box.get_x_lims()
        pane = pane_class(self.data, self.fs, self.resampled_data, self.resampled_fs, self.__delete_pane, x_left, x_right)
        # pane.update_graph_x_lims(x_left, x_right)
        self.layout_area.addWidget(pane)

    def __delete_pane(self, widget_object: QWidget):
        self.layout_area.removeWidget(widget_object)
        self.layout_area.update()
        widget_object.deleteLater()

    def _get_pane_list(self) -> List[Pane_Base]:
        widget_list = []
        for i in range(self.layout_area.count()):
            widget = self.layout_area.itemAt(i).widget()
            if isinstance(widget, Pane_Base):
                widget_list.append(widget)
        return widget_list
    
    def get_pane_name_list(self) -> List[str]:
        pane_widgets = self._get_pane_list()
        pane_name_list = []
        for pane in pane_widgets:
            pane_name = pane.get_pane_name()
            pane_name_list.append(pane_name)
        return pane_name_list

    def __update_plot_x_lims(self, x_left, x_right):
        panes = self._get_pane_list()
        print(panes)
        for pane in panes:
            pane.update_graph_x_lims(x_left, x_right)

    def set_second_channel_data(self, data, fs):
        self.second_data = data
        self.second_fs = fs
        self.second_channel_available = True

    def zoom_in(self):
        xlim = self.draggable_box.get_x_lims()
        # ylim = self.ax.get_ylim()
        center = (xlim[0] + xlim[1])/2

        x_left = center - (center - xlim[0]) * 0.9
        x_right = center + (xlim[1] - center) * 0.9
        self.draggable_box.set_x_lims(x_left, x_right)
        # self.ax.set_ylim(ylim[0] * 0.9, ylim[1] * 0.9)
        self.canvas_waveform.draw()

        self.__update_plot_x_lims(x_left, x_right)

    def zoom_out(self):
        xlim = self.draggable_box.get_x_lims()
        # ylim = self.ax.get_ylim()
        center = (xlim[0] + xlim[1])/2

        x_left = center - (center - xlim[0]) * 1.1
        x_left = max(0, x_left)
        x_right = center + (xlim[1] - center) * 1.1
        x_right = min(self._time, x_right)

        self.draggable_box.set_x_lims(x_left, x_right)
        # self.ax.set_ylim(ylim[0] * 1.1, ylim[1] * 1.1)
        self.canvas_waveform.draw()

        self.__update_plot_x_lims(x_left, x_right)

    def get_active_radio_button(self):
        radioButtons = [
            self.radioButton2,
            self.radioButton3,
            self.radioButton4,
            self.radioButton5,
            self.radioButton6,
            self.radioButton7,
            # self.radioButton8
            self.radioButton9,
            self.radioButton10,
            self.radioButton11,
        ]
        for button in radioButtons:
            if button.isChecked():
                return button.text()  # Returns the text of the checked radio button
        return None

    def export(self) -> List[mtAxes.Axes]:
        axes = []
        axes.append(self.ax_waveform)

        panes = self._get_pane_list()
        for pane in panes:
            axes.append(pane._ax)
        
        return axes
        # self.__print_window = PrintWindow(axes)
        # self.__print_window.show()
        
    def save_file(self, file_path):
        x_left, x_right = self.draggable_box.get_x_lims()

        config = {
            'data': self.data,
            'fs': self.fs,
            'resampled_data': self.resampled_data,
            'resampled_fs': self.resampled_fs,
            'plot_config': {
                'x_start': x_left,
                'x_end': x_right,
                'y_start': self.ax_waveform.get_ylim()[0],
                'y_end': self.ax_waveform.get_ylim()[1],
            },
            'other_plot_config': {
                'panes': [pane.get_pane_name() for pane in self._get_pane_list()],
            },
        }
        with open(file_path, 'wb') as file:
            pickle.dump(config, file)

    def load_file(self, file_path):
        with open(file_path, 'rb') as file:
            config = pickle.load(file)
        
        self.set_data(config['data'],config['fs'])
        
        x_left, x_right = config['plot_config']['x_start'], config['plot_config']['x_end']

        self.draggable_box.set_x_lims(x_left, x_right)

        # self.ax_waveform.set_xlim(config['plot_config']['x_start'], config['plot_config']['x_end'])
        # self.ax_waveform.set_ylim(config['plot_config']['y_start'], config['plot_config']['y_end'])
        for pane_name in config['other_plot_config']['panes']:
            self._add_pane(pane_name)
        self.canvas_waveform.draw()

class PrintWindow(QWidget):
    def __init__(self, axes):
        super().__init__()
        self.setWindowTitle("Export")
        self.showMaximized()

        self.__original_axes = axes
        self.__n_rows, self.__n_cols = len(self.__original_axes), len(self.__original_axes[0])

        # Create a matplotlib figure and canvas
        self.__figure, self.__axes = plt.subplots(self.__n_rows, self.__n_cols)
        self.__figure.set_constrained_layout(True)
        self.__canvas = FigureCanvas(self.__figure)

        for (old_ax, new_ax) in zip(utils.flatten_2d(self.__original_axes), utils.flatten_2d(self.__axes)):
            utils.copy_axes(old_ax, new_ax)
            # new_ax.set_xticks([])  # Remove x-ticks
            # new_ax.set_xlabel('')  # Remove x-axis title
            # new_ax.set_ylabel(new_ax.get_ylabel(), rotation=0)

        self.__preview_layout = QVBoxLayout()
        self.__preview_layout.addWidget(self.__canvas)
        self.__preview_box = QGroupBox('Preview')
        self.__preview_box.setLayout(self.__preview_layout)
        
        self.__title_input_layout = QVBoxLayout()

        self.__suptitle = QLineEdit(self)
        self.__suptitle.setPlaceholderText('Title...')
        self.__title_input_layout.addWidget(self.__suptitle)

        self.__ax_titles = []
        for index, _ in enumerate(utils.flatten_2d(self.__axes)):
            ax_title = QLineEdit(self)
            ax_title.setPlaceholderText(f'Graph {index}')
            self.__title_input_layout.addWidget(ax_title)
            self.__ax_titles.append(ax_title)

        self.__preview_button = QPushButton('Preview', self)
        self.__preview_button.clicked.connect(self.__update_title)
        self.__title_input_layout.addWidget(self.__preview_button)

        self.__title_input_layout
        self.__title_input_box = QGroupBox('Titles')
        self.__title_input_box.setLayout(self.__title_input_layout)

        layout = QHBoxLayout()
        layout.addWidget(self.__preview_box, 5)
        layout.addWidget(self.__title_input_box, 1)

        # layout.
        self.setLayout(layout)

    def __on_text_change(self):
        # Every time the text changes, reset and restart the timer
        self.timer.start(1000)

    def __update_title(self):
        suptitle = self.__suptitle.text()
        ax_titles_text = []
        for ax_title in self.__ax_titles:
            text = ax_title.text()
            ax_titles_text.append(text)
        
        def __background():
            self.__figure.suptitle(suptitle)
            for ax, ax_title_box in zip(self.__axes, self.__ax_titles):
                title = ax_title_box.text()
                ax.set_title(title)

            self.__canvas.draw()
        
        thread = threading.Thread(None, __background)
        thread.start()

class MainWindow(QMainWindow):
    def __init__(self, args):
        super().__init__()
        self.logs = []
        self.file_path = None
        self.file_base_name = None
        self.file_path_2 = None
        self.file_base_name_2 = None
        self.initUI()
        
        if(len(args) > 0):
            self.__load_file_from_args(args[0])

    def initUI(self):
        self.createFileMenu()
        self.createPaneMenu()
        self.createMoreMenu()

        central_widget = QWidget(self)
        self.main_layout = QVBoxLayout(central_widget)

        self.audio_layouts = QHBoxLayout()

        # self.left_component = AudioComponent()
        # self.right_component = AudioComponent()
        # self.splitter.addWidget(self.left_component)

        ## Code change: self.right_component is only added when the user want to load new file. 
        ## otherwise it wont even be rendered
        ## check loadFile for more info.
        # self.splitter.addWidget(self.right_component)

        self.main_layout.addLayout(self.audio_layouts, 9)

        footer_layout = QHBoxLayout()
        footer_layout.setAlignment(Qt.AlignCenter)

        trademark_label = QLabel("© Speech Processing Lab (SPL), IIITH. All rights reserved.")
        trademark_label.setAlignment(Qt.AlignCenter)
        footer_layout.addWidget(trademark_label)

        logo1 = QLabel()
        pixmap1 = QPixmap("../icons/iiith-2.png")  # Update with actual path
        scaled_pixmap = pixmap1.scaled(50, 50, Qt.KeepAspectRatio)
        logo1.setPixmap(scaled_pixmap)
        logo1.setAlignment(Qt.AlignCenter)
        footer_layout.addWidget(logo1)

        self.main_layout.addLayout(footer_layout)

        self.setCentralWidget(central_widget)

        self.setGeometry(100, 100, 800, 600)
        self.setWindowTitle('Waveform-Wizard')
        self.showMaximized()  # Start the application in full-screen mode

    def createFileMenu(self):
        file_menu = self.menuBar().addMenu('File')

        load_action = QAction('Load Single File', self)
        load_action.triggered.connect(self.__invoke_file_picker)
        file_menu.addAction(load_action)

        export_action = QAction('Export', self)
        export_action.triggered.connect(self.__invoke_export)
        file_menu.addAction(export_action)

        compare_action = QAction('Compare with File', self)
        compare_action.triggered.connect(self.compareFiles)
        file_menu.addAction(compare_action)

        save_action = QAction('Save File', self)
        save_action.triggered.connect(self.saveFile)
        file_menu.addAction(save_action)

    def createPaneMenu(self):
        pane_menu = self.menuBar().addMenu('Panes')

        # Add "Add Pane" option with a submenu
        add_pane_menu = pane_menu.addMenu("Add Pane")

        # List of graph pane names
        graph_pane_names = [
            'Waveform', 'Spectrogram', 'ZTWS', 'Gammatonegram', 
            'SFF', 'Formant Peaks', 'VAD', 'Pitch Contour', 
            'Constant-Q', 'EGG'
        ]

        for pane_name in graph_pane_names:
            add_pane_action = QAction(pane_name, self)

            # Call the __add_pane method from AudioComponent on left_component
            add_pane_action.triggered.connect(
                lambda checked, p=pane_name: self.__add_pane(p)
            )

            add_pane_menu.addAction(add_pane_action)

    def __get_audio_components(self) -> List[AudioComponent]:
        layout_list = []
        for i in range(self.audio_layouts.count()):
            widget = self.audio_layouts.itemAt(i).widget()
            if isinstance(widget, AudioComponent):
                layout_list.append(widget)
        return layout_list

    def __add_pane(self, pane_name):
        audio_component_list = self.__get_audio_components()

        for audio_component in audio_component_list:
            audio_component._add_pane(pane_name)

    def createMoreMenu(self):
        file_menu = self.menuBar().addMenu('More')

        about_action = QAction('About', self)
        about_action.triggered.connect(self.showAbout)
        file_menu.addAction(about_action)

    def createAlignmentAction(self, text, log_text):
        alignment_action = QAction(text, self)
        alignment_action.setCheckable(True)
        alignment_action.triggered.connect(lambda: self.change_pane(text))
        return alignment_action

    def __invoke_export(self):
        # TODO: Rewrite
        audio_component_list = self.__get_audio_components()
        
        axes_list = []
        for audio_component in audio_component_list:
            axes = audio_component.export()
            axes_list.append(axes)

        # Use * to unpack the lists for zip
        axes_grid = list(zip(*axes_list))
        axes_grid = [list(axes) for axes in axes_grid]

        self.__print_window = PrintWindow(axes_grid)
        self.__print_window.show()

        # self.left_component.export()

    def __get_existing_pane_list(self) -> List[str]:
        audio_component_list = self.__get_audio_components()
        if(len(audio_component_list) == 0):
            return []
        else:
            pane_name_list = audio_component_list[0].get_pane_name_list()
            return pane_name_list

    def __load_file_from_file_name(self, file_name):
        if(get_file_extension(file_name) not in ['wav', 'wwc']):
            show_error_message('File Format unsupported')
            return

        initial_pane_list=self.__get_existing_pane_list()

        if(get_file_extension(file_name) in ['wav']):
            self._log_action(f"Selected file: {file_name}")
            file_path = file_name
            file_base_name = os.path.basename(file_name)
            # self.refresh_left_area()

            data, samplerate = sf.read(file_path)

            first_data = process_audio(data)
            
            new_audio_component = AudioComponent(file_base_name)
            new_audio_component.set_data(first_data, samplerate)
            self.audio_layouts.addWidget(new_audio_component)
            
            if has_second_channel(data) == True:
                second_data = data[:, 1]
                new_audio_component.set_second_channel_data(second_data, samplerate)
            
            for pane in initial_pane_list:
                new_audio_component._add_pane(pane)

    def __load_file_from_args(self, arg_1):
        cwd = os.getcwd()
        resolved_path = os.path.abspath(os.path.join(cwd, arg_1))
        self.__load_file_from_file_name(resolved_path)
    def __save_file(self, file_path):
        audio_component_list = self.__get_audio_components()
        if len(audio_component_list) == 0:
            show_error_message('No file loaded')
            return

        all_configs = []

        # Iterate over each audio component and generate its config
        for component in audio_component_list:
            x_left, x_right = component.draggable_box.get_x_lims()
            config = {
                'file_name': component.file_name,
                'data': component.data,
                'fs': component.fs,
                'resampled_data': component.resampled_data,
                'resampled_fs': component.resampled_fs,
                'plot_config': {
                    'x_start': x_left,
                    'x_end': x_right,
                    'y_start': component.ax_waveform.get_ylim()[0],
                    'y_end': component.ax_waveform.get_ylim()[1],
                },
                'other_plot_config': {
                    'panes': [pane.get_pane_name() for pane in component._get_pane_list()],
                },
            }
            all_configs.append(config)

        # Dump all configs to the specified file as a pickle
        with open(file_path, 'wb') as f:
            pickle.dump(all_configs, f)
    
    def __load_file(self, file_path):
        with open(file_path, 'rb') as file:
            config = pickle.load(file)
        # console.log(config)
        for audio_component in config:
            new_audio_component = AudioComponent(audio_component['file_name'])
            new_audio_component.set_data(audio_component['data'], audio_component['fs'])
            new_audio_component.x_left = audio_component['plot_config']['x_start']
            new_audio_component.x_right = audio_component['plot_config']['x_end']
            new_audio_component.ax_waveform.set_ylim(audio_component['plot_config']['y_start'], audio_component['plot_config']['y_end'])
            for pane in audio_component['other_plot_config']['panes']:
                new_audio_component._add_pane(pane)
            self.audio_layouts.addWidget(new_audio_component)
        
            
        

    def __invoke_file_picker(self):
        options = QFileDialog.Options()
        fileName, _ = QFileDialog.getOpenFileName(self, "Load Single File", "", "All Files (*);;Text Files (*.txt)", options=options)
        if fileName:
            if self.file_path == None:
                if(get_file_extension(fileName) in ['wav']):
                    self.__load_file_from_file_name(fileName)
                else:
                    file_base_name = os.path.basename(fileName)
                    self.__load_file(fileName)
                    # new_audio_component = AudioComponent(file_base_name)
                    # new_audio_component.load_file(fileName)
                    # self.audio_layouts.addWidget(new_audio_component)
            else:
                show_error_message('Already viewing one file, open another window')
                return

    def compareFiles(self):
        pass
        # options = QFileDialog.Options()
        # fileName, _ = QFileDialog.getOpenFileName(self, "Load Single File", "", "All Files (*);;Text Files (*.txt)", options=options)
        
        # if fileName:
        #     if(get_file_extension(fileName) not in ['wav']):
        #         show_error_message('File Format unsupported')
        #         return

        #     if self.file_path_2 == None:
        #         self._log_action(f"Selected file: {fileName}")
        #         self.file_path_2 = fileName
        #         self.file_base_name_2 = os.path.basename(fileName)
        #         self.refresh_right_area()

        #         data, samplerate = sf.read(self.file_path_2)
        #         first_data = process_audio(data)
        #         self.right_component.set_data(first_data, samplerate)
                
        #         if has_second_channel(data) == True:
        #             second_data = data[:, 1]
        #             self.right_component.set_second_channel_data(second_data, samplerate)

        #         self.splitter.addWidget(self.right_component)
        #     else:
        #         show_error_message('Already viewing one file, open another window')
        #         return

    def saveFile(self):
        # # TODO: Rewrite
        # pass
        options = QFileDialog.Options()
        fileName, _ = QFileDialog.getSaveFileName(self, "Save As", "", "Custom Files (*.wwc);;All Files (*)", options=options)
        if fileName:
            if not fileName.endswith('.wwc'):
                fileName += '.wwc'
        self.__save_file(fileName)
            
        #     self.left_component.save_file(fileName)

    def showAbout(self):
        win = AboutInfoWindow(self)
        win.exec_()

    def _log_action(self, text):
        print(text)
        self.logs.append(text)

if __name__ == '__main__':
    appctxt = ApplicationContext()
    mainWindow = MainWindow(args=sys.argv[1:])
    mainWindow.show()
    # This fixes the issue with PySide2 that the exec function is not found
    exec_func = getattr(appctxt.app, 'exec', appctxt.app.exec_)
    sys.exit(exec_func())