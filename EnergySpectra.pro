TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += chartlib spectrumlib app

app.depends = chartlib spectrumlib
