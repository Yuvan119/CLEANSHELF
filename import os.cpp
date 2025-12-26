import os
import psutil
import tkinter as tk
from tkinter import messagebox
import subprocess

# Automatically detect system drive (C:)
DRIVE = os.path.abspath(os.sep)  # Usually C:\ on Windows

def get_drive_usage(drive_path):
    usage = psutil.disk_usage(drive_path)
    total_gb = usage.total / (1024 ** 3)
    used_gb = usage.used / (1024 ** 3)
    free_gb = usage.free / (1024 ** 3)
    return total_gb, used_gb, free_gb

def ask_cleanup():
    total_gb, used_gb, free_gb = get_drive_usage(DRIVE)

    message = (
        f"Drive: {DRIVE}\n\n"
        f"Total Storage: {total_gb:.2f} GB\n"
        f"Used Storage: {used_gb:.2f} GB\n"
        f"Free Storage: {free_gb:.2f} GB\n\n"
        "We found unused files taking up space.\n"
        "Do you want to clean them now?"
    )

    root = tk.Tk()
    root.withdraw()
    result = messagebox.askyesno("Storage Analyzer", message)

    if result:
        try:
           subprocess.Popen(["pythonw", "cleanshelf.py"])
  # Change file name
        except Exception as e:
            messagebox.showerror("Error", f"Could not run cleanup script:\n{e}")
    else:
        messagebox.showinfo(
            "Reminder Set",
            "Okay! We'll remind you again next week."
        )

if __name__ == "__main__":
    ask_cleanup()
