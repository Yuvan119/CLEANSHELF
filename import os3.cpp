import os
import time
import tkinter as tk
from tkinter import messagebox
import send2trash

# ----- SETTINGS -----
FOLDER_TO_SCAN = r"C:\Users\YourName\Downloads"  # Change to your folder
DAYS_UNUSED = 30  # Unused if not accessed in these many days

# ----- FIND UNUSED FILES -----
def find_unused_files(folder, days):
    unused_files = []
    now = time.time()
    cutoff = now - (days * 86400)  # Convert days to seconds

    for root, dirs, files in os.walk(folder):
        for file in files:
            path = os.path.join(root, file)
            last_access = os.path.getatime(path)
            if last_access < cutoff:
                unused_files.append(path)
    return unused_files

# ----- DELETE SELECTED FILES -----
def delete_selected():
    selected_files = [file for file, var in checkboxes if var.get()]
    if not selected_files:
        messagebox.showwarning("No Selection", "Please select at least one file to delete.")
        return

    for file in selected_files:
        send2trash.send2trash(file)

    messagebox.showinfo("Done", f"Deleted {len(selected_files)} file(s) to Recycle Bin.")
    root.destroy()

# ----- MAIN -----
unused_files = find_unused_files(FOLDER_TO_SCAN, DAYS_UNUSED)

root = tk.Tk()
root.title("CleanShelf - Select Files to Delete")
root.geometry("700x500")

frame = tk.Frame(root)
frame.pack(fill="both", expand=True)

tk.Label(frame, text=f"Unused files (not accessed in {DAYS_UNUSED} days):", font=("Arial", 12, "bold")).pack(anchor="w")

checkboxes = []
for file in unused_files:
    var = tk.BooleanVar()
    cb = tk.Checkbutton(frame, text=file, variable=var, anchor="w", wraplength=650, justify="left")
    cb.pack(fill="x", anchor="w")
    checkboxes.append((file, var))

btn_delete = tk.Button(root, text="Delete Selected", command=delete_selected, bg="red", fg="white")
btn_delete.pack(pady=10)

root.mainloop()
