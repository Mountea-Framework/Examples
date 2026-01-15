import tkinter as tk
from tkinter import filedialog, messagebox
import os
from pathlib import Path
import json


bold_font = ("Roboto", 12, "bold")
regular_font = ("Roboto", 10)


class ToolTip:
    
    def __init__(self, widget):
        self.widget = widget
        self.tip_window = None
        self.x = self.y = 0

    def show_tip(self, text, index):
        if self.tip_window or not text:  # Fix: changed self.text to text
            return
        x, y, width, height = self.widget.bbox(index)
        x += self.widget.winfo_rootx() + 25
        y += self.widget.winfo_rooty() + 20
        self.tip_window = tw = tk.Toplevel(self.widget)
        tw.wm_overrideredirect(True)
        tw.wm_geometry("+%d+%d" % (x, y))
        label = tk.Label(tw, text=text, justify=tk.LEFT,
                         background="#ffffe0", relief=tk.SOLID,
                         borderwidth=1, font=("tahoma", "8", "normal"))
        label.pack(ipadx=1)

    def hide_tip(self):
        tw = self.tip_window
        self.tip_window = None

        if tw:
            tw.destroy()


def on_enter(event):
    if event.widget.cget('state') != 'disabled':
        event.widget.config(cursor="hand2")

def on_leave(event):
    event.widget.config(cursor="")

def create_button(parent_frame, text, command, inputState='normal', **kwargs):
    button = tk.Button(parent_frame, text=text, command=command, font=regular_font, state=inputState, **kwargs)
    
    button.bind("<Enter>", on_enter)  # Bind mouse entering event
    button.bind("<Leave>", on_leave)  # Bind mouse leaving event
    return button