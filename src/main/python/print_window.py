import threading

import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt5agg import \
    FigureCanvasQTAgg as FigureCanvas

from PyQt5.QtWidgets import (QGroupBox, QHBoxLayout,  QVBoxLayout, QWidget,
                             QLineEdit, QPushButton, QGridLayout, QComboBox,
                             QFileDialog, QMessageBox, QLabel)

import utils

class Column_Titles(QWidget):
    def __init__(self, index):
        super().__init__()
        self.__index = index

        self.__col_title = QLineEdit(self)
        self.__col_title.setPlaceholderText(f'Col {self.__index} Title')
        
        self.__layout = QVBoxLayout()
        self.__layout.addWidget(self.__col_title)
        self.setLayout(self.__layout)
    
    def get_value(self):
        return self.__col_title.text()

class Row_Titles(QGroupBox):
    def __init__(self, index):
        super().__init__(title=f'Row {index}')
        self.__index = index

        self.__row_title_left = QLineEdit(self)
        self.__row_title_left.setPlaceholderText(f'Row {self.__index} Title (Left)')

        self.__row_title_right = QLineEdit(self)
        self.__row_title_right.setPlaceholderText(f'Row {self.__index} Title (Right)')
        
        self.__layout = QVBoxLayout()
        self.__layout.addWidget(self.__row_title_left)
        self.__layout.addWidget(self.__row_title_right)
        self.setLayout(self.__layout)
    
    def get_value(self):
        return self.__row_title_left.text(), self.__row_title_right.text()

class ExportPopup(QWidget):
    def __init__(self, fig):
        super().__init__()

        self.__fig = fig
        self.setWindowTitle("Export Options")
        # self.setFixedSize(600, 400)
        self.setFixedWidth(600)
        self.setMinimumHeight(100)

        self.__layout = QVBoxLayout()

        self.__cancel_button = QPushButton('Cancel', self)
        self.__cancel_button.clicked.connect(self.__cancel)

        self.__location_picker = QPushButton('Select File Location', self)
        self.__location_picker.clicked.connect(self.__file_picker)

        self.__export_format_selector = QComboBox()
        self.__export_format_selector.addItems(['.eps', '.png', '.pdf', '.jpg', '.jpeg', '.svg'])
        
        self.__export_button = QPushButton('Export', self)
        self.__export_button.clicked.connect(self.__export)

        self.__layout.addWidget(self.__export_format_selector)
        self.__layout.addWidget(self.__location_picker)
        self.__layout.addWidget(self.__export_button)
        self.__layout.addWidget(self.__cancel_button)

        self.setLayout(self.__layout)

    def __cancel(self):
        self.close()

    def __export(self):
        file_name = self.__file_name
        
        self.__fig.savefig(file_name)

        message = "File saved successfully"
        msg = QMessageBox()
        msg.setIcon(QMessageBox.Icon.Information)
        msg.setText(message)
        msg.setWindowTitle("Succesful")
        msg.exec_()

        self.close()

    def __file_picker(self):
        file_format = self.__export_format_selector.currentText()

        options = QFileDialog.Options()
        fileName, _ = QFileDialog.getSaveFileName(self, "Save As", "", f"Custom Files (*{file_format});;All Files (*)", options=options)

        if fileName == None or len(fileName) == 0:
            self.__cancel()

        if not fileName.endswith(file_format):
            fileName += file_format
        
        self.__file_name = fileName

        
        self.__location_picker.hide()
        self.__new_location_picker = QLabel(f"Path: {self.__file_name}")

        self.__layout.insertWidget(self.__layout.indexOf(self.__location_picker), self.__new_location_picker)
        self.__layout.removeWidget(self.__location_picker)
        self.__new_location_picker.show()

class PrintWindow(QWidget):
    def __init__(self, axes):
        super().__init__()
        self.setWindowTitle("Export")
        self.showMaximized()

        self.__original_axes = axes
        self.__n_rows, self.__n_cols = len(self.__original_axes), len(self.__original_axes[0])

        # Create a matplotlib figure and canvas
        self.__figure, self.__axes = plt.subplots(self.__n_rows, self.__n_cols, squeeze=False)
        self.__figure.set_constrained_layout(True)
        self.__canvas = FigureCanvas(self.__figure)

        for (old_ax, new_ax) in zip(utils.flatten_2d(self.__original_axes), utils.flatten_2d(self.__axes)):
            utils.copy_axes(old_ax, new_ax)
            # new_ax.set_xticks([])  # Remove x-ticks
            # new_ax.set_xlabel('')  # Remove x-axis title
            # new_ax.set_ylabel(new_ax.get_ylabel(), rotation=0)
        
        self.__col_title_input_layout = QHBoxLayout()
        self.__row_title_input_layout = QVBoxLayout()
        self.__main_title_input_layout = QVBoxLayout()
        self.__preview_box = QGroupBox('Preview')
        self.__action_layout = QHBoxLayout()

        self.__preview_layout = QVBoxLayout()
        self.__preview_layout.addWidget(self.__canvas)
        self.__preview_box.setLayout(self.__preview_layout)
    
        self.__suptitle = QLineEdit(self)
        self.__suptitle.setPlaceholderText('Title...')
        self.__main_title_input_layout.addWidget(self.__suptitle)

        self.__col_title_list = []
        for index in range(self.__n_cols):
            col_title = Column_Titles(index)
            self.__col_title_input_layout.addWidget(col_title)
            self.__col_title_list.append(col_title)
        
        self.__row_title_list = []
        for index in range(self.__n_rows):
            row_title = Row_Titles(index)
            self.__row_title_input_layout.addWidget(row_title)
            self.__row_title_list.append(row_title)

        self.__preview_button = QPushButton('Preview', self)
        self.__preview_button.clicked.connect(self.__update_title)
        
        self.__cancel_button = QPushButton('Cancel', self)
        self.__cancel_button.clicked.connect(self.__cancel)

        self.__export_button = QPushButton('Export', self)
        self.__export_button.clicked.connect(self.__export)

        self.__action_layout.addWidget(self.__preview_button)
        # self.__action_layout.addWidget(self.__export_format_selector)
        self.__action_layout.addWidget(self.__export_button)

        layout = QGridLayout()
        layout.addLayout(self.__main_title_input_layout, 0, 0)
        layout.addLayout(self.__col_title_input_layout, 0, 1)
        layout.addLayout(self.__row_title_input_layout, 1, 0)
        layout.addWidget(self.__preview_box, 1, 1)
        layout.addLayout(self.__action_layout, 2, 0, 1, 2)

        layout.setRowStretch(0, 1)
        layout.setRowStretch(1, 8)
        layout.setRowStretch(2, 1)
        layout.setColumnStretch(0, 1)
        layout.setColumnStretch(1, 5)

        # layout.
        self.setLayout(layout)

    def __on_text_change(self):
        # Every time the text changes, reset and restart the timer
        self.timer.start(1000)

    def __update_title(self):
        suptitle = self.__suptitle.text()
        
        col_titles_text = []
        for col_title in self.__col_title_list:
            text = col_title.get_value()
            col_titles_text.append(text)
        
        row_titles_text = []
        for row_title in self.__row_title_list:
            text_left, text_right = row_title.get_value()
            row_titles_text.append([text_left, text_right])

        def __background():
            self.__figure.suptitle(suptitle)

            for j in range(self.__n_cols):
                top_most_axes = self.__axes[0][j]
                top_most_axes.set_title(col_titles_text[j])
            
                # self.__figure.text((j + 0.5) / self.__n_cols, 0.93, f'Column {j + 1}', ha='center', fontsize=14)

            # Add row titles
            for i in range(self.__n_rows):
                left_most_axes = self.__axes[i][0]
                right_most_axes = self.__axes[i][-1]

                left_most_axes.set_ylabel(row_titles_text[i][0])

                right_most_axes.yaxis.set_label_position("right")
                right_most_axes.set_ylabel(row_titles_text[i][1])
                # self.__figure.text(0.02, (self.__n_rows - i - 0.5) / self.__n_rows, f'Row {i + 1}', va='center', rotation='vertical', fontsize=14)

            self.__canvas.draw()
        
        thread = threading.Thread(None, __background)
        thread.start()
    
    def __cancel(self):
        self.close()
    
    def __export(self):
        self.__export_window = ExportPopup(self.__figure)
        self.__export_window.show()