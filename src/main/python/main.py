import os
import sys
import threading
from datetime import datetime
from time import sleep
from typing import List

import matplotlib.pyplot as plt
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
                             QTextEdit, QToolButton, QVBoxLayout, QWidget, QDialog, QPushButton)
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
    def __init__(self):
        super().__init__("Select a File to continue")
        self.initUI()

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

        # spect_pane = Pane_Factory.get_pane_class_by_name('Spectrogram')(self.data, self.fs, self.resampled_data, self.resampled_fs)
        # self.layout_area.addWidget(spect_pane)
        # ztws_pane = Pane_Factory.get_pane_class_by_name('ZTWS')(self.data, self.fs, self.resampled_data, self.resampled_fs)
        # self.layout_area.addWidget(ztws_pane)
        # gammatonegram_pane = Pane_Factory.get_pane_class_by_name('Gammatonegram')(self.data, self.fs, self.resampled_data, self.resampled_fs)
        # self.layout_area.addWidget(gammatonegram_pane)
        # sff_pane = Pane_Factory.get_pane_class_by_name('SFF')(self.data, self.fs, self.resampled_data, self.resampled_fs)
        # self.layout_area.addWidget(sff_pane)
        # formatPeaks_pane = Pane_Factory.get_pane_class_by_name('Formant Peaks')(self.data, self.fs, self.resampled_data, self.resampled_fs)
        # self.layout_area.addWidget(formatPeaks_pane)
        # vad_pane = Pane_Factory.get_pane_class_by_name('VAD')(self.data, self.fs, self.resampled_data, self.resampled_fs)
        # self.layout_area.addWidget(vad_pane)
        # contour_pane = Pane_Factory.get_pane_class_by_name('Pitch Contour')(self.data, self.fs, self.resampled_data, self.resampled_fs)
        # self.layout_area.addWidget(contour_pane)
        # constantq_pane = Pane_Factory.get_pane_class_by_name('Constant-Q')(self.data, self.fs, self.resampled_data, self.resampled_fs)
        # self.layout_area.addWidget(constantq_pane)
        # egg_pane = Pane_Factory.get_pane_class_by_name('EGG')(self.data, self.fs, self.resampled_data, self.resampled_fs)
        # self.layout_area.addWidget(egg_pane)

        self.__add_pane('Waveform')
        self.__add_pane('Spectrogram')
        self.__add_pane('ZTWS')
        self.__add_pane('Gammatonegram')
        self.__add_pane('SFF')
        self.__add_pane('Formant Peaks')
        self.__add_pane('VAD')
        self.__add_pane('Pitch Contour')
        self.__add_pane('Constant-Q')
        self.__add_pane('EGG')

    def __add_pane(self, pane_name):
        pane_class = Pane_Factory.get_pane_class_by_name(pane_name)
        pane = pane_class(self.data, self.fs, self.resampled_data, self.resampled_fs, self.__delete_pane)
        self.layout_area.addWidget(pane)

    def __delete_pane(self, widget_object: QWidget):
        self.layout_area.removeWidget(widget_object)
        self.layout_area.update()
        widget_object.deleteLater()

    def __get_pane_list(self) -> List[Pane_Base]:
        widget_list = []
        for i in range(self.layout_area.count()):
            widget = self.layout_area.itemAt(i).widget()
            if isinstance(widget, Pane_Base):
                widget_list.append(widget)
        return widget_list
    
    def __update_plot_x_lims(self, x_left, x_right):
        panes = self.__get_pane_list()
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

    def save_file(self, file_path):
        config = {
            'data': self.data,
            'fs': self.fs,
            'resampled_data': self.resampled_data,
            'resampled_fs': self.resampled_fs,
            'plot_config': {
                'x_start': self.ax_waveform.get_xlim()[0],
                'x_end': self.ax_waveform.get_xlim()[1],
                'y_start': self.ax_waveform.get_ylim()[0],
                'y_end': self.ax_waveform.get_ylim()[1],
            },
            'other_plot_config': {
                'plot': self.get_active_radio_button(),
                'x_start': self.ax_other.get_xlim()[0],
                'x_end': self.ax_other.get_xlim()[1],
                'y_start': self.ax_other.get_ylim()[0],
                'y_end': self.ax_other.get_ylim()[1],
            }
        }
        with open(file_path, 'wb') as file:
            pickle.dump(config, file)

    def load_file(self, file_path):
        with open(file_path, 'rb') as file:
            config = pickle.load(file)
        
        self.set_data(config['data'],config['fs'])
        
        self.ax_waveform.set_xlim(config['plot_config']['x_start'], config['plot_config']['x_end'])
        self.ax_waveform.set_ylim(config['plot_config']['y_start'], config['plot_config']['y_end'])
        self.canvas_waveform.draw()

        mp = {
            'Spectogram': [self.radioButton2, self.update_spectogram_plot],
            'Zero Time Wind Spectrum': [self.radioButton3, self.update_zero_time_wind_spectrum_plot],
            'Gammatonegram': [self.radioButton4, self.update_gammatonegram_plot],
            'sff': [self.radioButton5, self.update_single_freq_filter_fs],
            'Formant Peaks': [self.radioButton6, self.update_formant_peaks],
            'VAD': [self.radioButton7, self.update_vad_plot],
            'Pitch Contours': [self.radioButton9, self.update_pitch_contour_plot],
            'Constant-Q': [self.radioButton10, self.update_constantq_plot],
            'EGG': [self.radioButton11, self.update_egg_plot],
        }

        plot_used = config['other_plot_config']['plot']
        btn, fn = mp[plot_used]
        btn.setChecked(True)

        def callbk():
            self.ax_other.set_xlim(config['other_plot_config']['x_start'], config['other_plot_config']['x_end'])
            self.ax_other.set_ylim(config['other_plot_config']['y_start'], config['other_plot_config']['y_end'])
            self.canvas_other.draw()

        self.update_in_background(fn, callbk)()

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
        self.createOrientationMenu()
        self.createMoreMenu()

        central_widget = QWidget(self)
        main_layout = QVBoxLayout(central_widget)

        self.splitter = QSplitter(self)

        self.left_component = AudioComponent()
        self.right_component = AudioComponent()
        self.splitter.addWidget(self.left_component)

        ## Code change: self.right_component is only added when the user want to load new file. 
        ## otherwise it wont even be rendered
        ## check loadFile for more info.
        # self.splitter.addWidget(self.right_component)

        main_layout.addWidget(self.splitter)

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

        main_layout.addLayout(footer_layout)

        self.setCentralWidget(central_widget)

        self.setGeometry(100, 100, 800, 600)
        self.setWindowTitle('Waveform-Wizard')
        self.showMaximized()  # Start the application in full-screen mode

    def createFileMenu(self):
        file_menu = self.menuBar().addMenu('File')

        load_action = QAction('Load Single File', self)
        load_action.triggered.connect(self.__invoke_file_picker)
        file_menu.addAction(load_action)

        compare_action = QAction('Compare with File', self)
        compare_action.triggered.connect(self.compareFiles)
        file_menu.addAction(compare_action)

        save_action = QAction('Save File', self)
        save_action.triggered.connect(self.saveFile)
        file_menu.addAction(save_action)

    def createOrientationMenu(self):
        orientation_menu = self.menuBar().addMenu('Orientation')

        swap_files = QAction('Swap Files', self)
        swap_files.triggered.connect(lambda: self._log_action('swapping...'))
        orientation_menu.addAction(swap_files)

        horizontal_alignment_action = self.createAlignmentAction('Horizontal', 'horizontal alignment')
        vertical_alignment_action = self.createAlignmentAction('Vertical', 'vertical alignment')

        separator_action = QAction('Alignment', self)
        separator_action.setSeparator(True)

        orientation_menu.addAction(separator_action)
        orientation_menu.addAction(horizontal_alignment_action)
        orientation_menu.addAction(vertical_alignment_action)

        alignment_actions = QActionGroup(self)
        alignment_actions.addAction(horizontal_alignment_action)
        alignment_actions.addAction(vertical_alignment_action)
        horizontal_alignment_action.setChecked(True)

    def createMoreMenu(self):
        file_menu = self.menuBar().addMenu('More')

        about_action = QAction('About', self)
        about_action.triggered.connect(self.showAbout)
        file_menu.addAction(about_action)

    def createAlignmentAction(self, text, log_text):
        alignment_action = QAction(text, self)
        alignment_action.setCheckable(True)
        alignment_action.triggered.connect(lambda: self.change_orientation(text))
        return alignment_action

    def change_orientation(self, text):
        if(text == 'Vertical'):
            self.splitter.setOrientation(Qt.Vertical)
        else:
            self.splitter.setOrientation(Qt.Horizontal)
        print('inside, ', text)

    def __load_file_from_file_name(self, file_name):
        if(get_file_extension(file_name) not in ['wav', 'wwc']):
            show_error_message('File Format unsupported')
            return

        if(get_file_extension(file_name) in ['wav']):
            self._log_action(f"Selected file: {file_name}")
            self.file_path = file_name
            self.file_base_name = os.path.basename(file_name)
            self.refresh_left_area()

            data, samplerate = sf.read(self.file_path)

            first_data = process_audio(data)
            self.left_component.set_data(first_data, samplerate)
            
            if has_second_channel(data) == True:
                second_data = data[:, 1]
                self.left_component.set_second_channel_data(second_data, samplerate)

    def __load_file_from_args(self, arg_1):
        cwd = os.getcwd()
        resolved_path = os.path.abspath(os.path.join(cwd, arg_1))
        self.__load_file_from_file_name(resolved_path)

    def __invoke_file_picker(self):
        options = QFileDialog.Options()
        fileName, _ = QFileDialog.getOpenFileName(self, "Load Single File", "", "All Files (*);;Text Files (*.txt)", options=options)
        if fileName:
            if self.file_path == None:
                if(get_file_extension(fileName) in ['wav']):
                    self.__load_file_from_file_name(fileName)
                else:
                    self.left_component.load_file(fileName)
            else:
                show_error_message('Already viewing one file, open another window')
                return

    def compareFiles(self):
        options = QFileDialog.Options()
        fileName, _ = QFileDialog.getOpenFileName(self, "Load Single File", "", "All Files (*);;Text Files (*.txt)", options=options)
        
        if fileName:
            if(get_file_extension(fileName) not in ['wav']):
                show_error_message('File Format unsupported')
                return

            if self.file_path_2 == None:
                self._log_action(f"Selected file: {fileName}")
                self.file_path_2 = fileName
                self.file_base_name_2 = os.path.basename(fileName)
                self.refresh_right_area()

                data, samplerate = sf.read(self.file_path_2)
                first_data = process_audio(data)
                self.right_component.set_data(first_data, samplerate)
                
                if has_second_channel(data) == True:
                    second_data = data[:, 1]
                    self.right_component.set_second_channel_data(second_data, samplerate)

                self.splitter.addWidget(self.right_component)
            else:
                show_error_message('Already viewing one file, open another window')
                return

    def saveFile(self):
        options = QFileDialog.Options()
        fileName, _ = QFileDialog.getSaveFileName(self, "Save As", "", "Custom Files (*.wwc);;All Files (*)", options=options)
        if fileName:
            if not fileName.endswith('.wwc'):
                fileName += '.wwc'
            
            self.left_component.save_file(fileName)

    def showAbout(self):
        win = AboutInfoWindow(self)
        win.exec_()

    def refresh_left_area(self):
        self.left_component.setTitle(self.file_base_name)

    def refresh_right_area(self):
        self.right_component.setTitle(self.file_base_name_2)

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