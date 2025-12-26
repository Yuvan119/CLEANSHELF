import os
import time
import tkinter as tk
from tkinter import messagebox
import subprocess
import psutil

# Set unused file rule (30 days in seconds)
DAYS_UNUSED = 30
SECONDS_UNUSED = DAYS_UNUSED * 24 * 60 * 60

# Get all drives
def get_drives():
    partitions = psutil.disk_partitions(all=False)
    return [p.device for p in partitions]

# Get total size of unused files
def get_unused_size(path):
    total_size = 0
    cutoff_time = time.time() - SECONDS_UNUSED
    for dirpath, dirnames, filenames in os.walk(path, topdown=True):
        dirnames[:] = [d for d in dirnames if not d.startswith("$Recycle.Bin")]
        for f in filenames:
            try:
                fp = os.path.join(dirpath, f)
                if os.path.isfile(fp):
                    last_access = os.path.getatime(fp)
                    if last_access < cutoff_time:
                        total_size += os.path.getsize(fp)
            except (PermissionError, FileNotFoundError, OSError):
                continue
    return total_size

# Main analysis
def analyze_storage():
    total_space = 0
    used_space = 0
    free_space = 0
    unused_files_size = 0

    for drive in get_drives():
        try:
            usage = psutil.disk_usage(drive)
            total_space += usage.total
            used_space += usage.used
            free_space += usage.free
            unused_files_size += get_unused_size(drive)
        except PermissionError:
            continue

    return total_space, used_space, free_space, unused_files_size

# Show dialog
def show_dialog():
    total, used, free, unused = analyze_storage()

    total_gb = total / (1024**3)
    used_gb = used / (1024**3)
    free_gb = free / (1024**3)
    unused_gb = unused / (1024**3)

    total_mb = total / (1024**2)
    used_mb = used / (1024**2)
    free_mb = free / (1024**2)
    unused_mb = unused / (1024**2)

    message = (
        f"📊 Storage Analysis Result:\n\n"
        f"Total Storage: {total_gb:.2f} GB ({total_mb:.2f} MB)\n"
        f"Used Storage: {used_gb:.2f} GB ({used_mb:.2f} MB)\n"
        f"Free Storage: {free_gb:.2f} GB ({free_mb:.2f} MB)\n"
        f"Unused Files (>{DAYS_UNUSED} days old): {unused_gb:.2f} GB ({unused_mb:.2f} MB)\n\n"
        "Do you want to clean unused files now?"
    )

    root = tk.Tk()
    root.withdraw()
    result = messagebox.askyesno("Storage Analyzer", message)

    if result:
        try:
            subprocess.Popen(["python", "your_cleanup_script.py"])  # Change script name
        except Exception as e:
            messagebox.showerror("Error", f"Could not run cleanup script:\n{e}")

if __name__ == "__main__":
    show_dialog()
