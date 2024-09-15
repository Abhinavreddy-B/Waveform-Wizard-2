# Waveform-Wizard

Waveform-Wizard is a simple, fast and easy to use application to analyse sound signals

# Installation

## Ubuntu

Download the latest installation files from [Releases](https://github.com/Abhinavreddy-B/Waveform-Wizard-2/releases) Page.

* For ubuntu 20.04 -> Waveform-Wizard_ubuntu-20_04.deb
* For ubuntu 22.04 -> Waveform-Wizard_ubuntu-22_04.deb

Open the `.deb` with ubuntu's software installer.

(or)

In terminal, go to the directory containing the downloaded file and run:

```sh
$ sudo apt install ./Waveform-Wizard_ubuntu-<version>.deb
```

# TODO:

## AboutInfoWindow:

- [x] add version info from from github actions.
- [ ] IIITH and SPL logos add cheyyacchu
- [ ] inkemanna details add cheyyacchu
- [ ] move to components/views

## AudioComponent:

- [ ] Move file loading and file saving methods into this class instead of mainWindow, (file picker logic kuda indole cheseyyali)
- [ ] Refactor to accommodate multiple graphs, list of plots instead of single plot_other, ax_other, canvas_other.
- [ ] removing radio buttons, instead to add new plots, add an option in tool bar option: Pane -> New Pane -> plot name. 
- [ ] While refactoring, better to have a base class. and all the plots will inherit some common functionality from base class. these  class objects of each plot can be put in components folder.
- [ ] save file an load file can should be changed to support multiplot
- [ ] Zoom in, Zoom out, move left, move right of the ax_waveform should change all the other plots to that x range and y range.

## MyMainWindow:

- [ ] rename the class to a better name
- [ ] Keep only 1 file per main window
- [ ] If a user tries to load another file, or wants to use 2 files at a time, just initialise another window(another application instance)
- [ ] accordingly remove the splitter (no longer required since we dont compare 2 files at a time)
- [ ] Remove orientation menu
- [ ] Add/ remove new options in File menu
- [ ] add Pane menu, with options to create new pane, delete pane, etc
- [ ] can add export menu either as a seperate menu or inside File menu

## Misc features:

Memory optimization:

- [ ] equate variables to None after using them, so that python can collect garbage
- [ ] use floats with less decimals to optimize for memory
- [ ] maybe look for matplotlib alternatives.

others:

- [ ] Loading '.wav' files directly, like in windows on clicking any movies, we get 'open with vlc' , etc right. ala emanna. (not sure if this is feasible)
