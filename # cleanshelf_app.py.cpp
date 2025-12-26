# cleanshelf_app.py
import os
import shutil
import json
import time
from datetime import datetime, timedelta
import tkinter as tk
from tkinter import ttk, messagebox, filedialog

# -------- CONFIG ----------
DAYS_UNUSED = 60                   # consider a file unused if not accessed in X days
RECYCLE_RETENTION_DAYS = 20        # permanently delete from custom recycle after this many days
LOG_FILENAME = "cleanshelf_recycle_log.json"
# --------------------------

def get_custom_recycle_folder():
    return os.path.join(os.path.expanduser("~"), "CleanShelfRecycle")

def ensure_recycle_folder():
    path = get_custom_recycle_folder()
    os.makedirs(path, exist_ok=True)
    return path

def load_deleted_log():
    if os.path.exists(LOG_FILENAME):
        try:
            with open(LOG_FILENAME, "r") as f:
                return json.load(f)
        except:
            return {}
    return {}

def save_deleted_log(d):
    try:
        with open(LOG_FILENAME, "w") as f:
            json.dump(d, f, indent=2)
    except Exception as e:
        print("Failed to save log:", e)

def is_unused(path, days_limit):
    try:
        last_access = os.path.getatime(path)
    except Exception:
        return False
    cutoff = time.time() - days_limit * 86400
    return last_access < cutoff

def scan_folder_for_unused(folder, days_limit=DAYS_UNUSED):
    results = []
    if not folder or not os.path.isdir(folder):
        return results
    for root, dirs, files in os.walk(folder):
        for name in files:
            path = os.path.join(root, name)
            try:
                if is_unused(path, days_limit):
                    last_access_ts = os.path.getatime(path)
                    last_access_str = datetime.fromtimestamp(last_access_ts).strftime("%Y-%m-%d %H:%M:%S")
                    results.append((name, last_access_str, path))
            except Exception:
                # skip unreadable files
                continue
    return results

def move_to_custom_recycle(paths):
    recycle = ensure_recycle_folder()
    log = load_deleted_log()
    moved = 0
    for p in paths:
        try:
            base = os.path.basename(p)
            dest = os.path.join(recycle, base)
            # if dest exists, avoid overwrite by appending timestamp
            if os.path.exists(dest):
                name, ext = os.path.splitext(base)
                dest = os.path.join(recycle, f"{name}_{int(time.time())}{ext}")
            shutil.move(p, dest)
            log[dest] = datetime.now().strftime("%Y-%m-%d")
            moved += 1
        except Exception as e:
            print(f"Failed to move {p}: {e}")
    save_deleted_log(log)
    return moved

def purge_old_recycle_files_gui(show_message=True):
    recycle = ensure_recycle_folder()
    log = load_deleted_log()
    now = datetime.now()
    updated_log = {}
    removed = 0
    for dest, deleted_date in log.items():
        try:
            del_dt = datetime.strptime(deleted_date, "%Y-%m-%d")
        except Exception:
            # keep malformed entries
            updated_log[dest] = deleted_date
            continue
        # if file still exists in recycle folder and older than retention, remove permanently
        if os.path.exists(dest):
            if (now - del_dt).days >= RECYCLE_RETENTION_DAYS:
                try:
                    os.remove(dest)
                    removed += 1
                except Exception as e:
                    print(f"Could not remove {dest}: {e}")
                    updated_log[dest] = deleted_date
            else:
                updated_log[dest] = deleted_date
        else:
            # if file missing (maybe user manually restored), don't keep entry
            continue
    save_deleted_log(updated_log)
    if show_message:
        messagebox.showinfo("Purge Complete", f"Permanently removed {removed} file(s) older than {RECYCLE_RETENTION_DAYS} days.")

# ---------------- GUI -----------------
class CleanShelfApp:
    def __init__(self, root):
        self.root = root
        self.root.title("CleanShelf - Organized File Manager")
        self.root.geometry("1000x600")

        self.folder_path = tk.StringVar()
        self.create_widgets()
        ensure_recycle_folder()

    def create_widgets(self):
        top = tk.Frame(self.root)
        top.pack(fill="x", padx=8, pady=6)

        tk.Label(top, text="Folder to Scan:").pack(side="left")
        self.entry = tk.Entry(top, textvariable=self.folder_path, width=70)
        self.entry.pack(side="left", padx=6)
        tk.Button(top, text="Browse", command=self.browse_folder).pack(side="left", padx=4)
        tk.Button(top, text="Scan for Unused Files", command=self.scan_and_populate).pack(side="left", padx=6)

        # Table area
        mid = tk.Frame(self.root)
        mid.pack(fill="both", expand=True, padx=8, pady=(0,6))

        columns = ("File Name", "Last Accessed", "Full Path")
        self.tree = ttk.Treeview(mid, columns=columns, show="headings", selectmode="extended")
        for col in columns:
            self.tree.heading(col, text=col)
            # give sensible widths
            if col == "File Name":
                self.tree.column(col, width=220, anchor="w")
            elif col == "Last Accessed":
                self.tree.column(col, width=150, anchor="center")
            else:
                self.tree.column(col, width=600, anchor="w")

        vsb = ttk.Scrollbar(mid, orient="vertical", command=self.tree.yview)
        hsb = ttk.Scrollbar(mid, orient="horizontal", command=self.tree.xview)
        self.tree.configure(yscroll=vsb.set, xscroll=hsb.set)
        self.tree.grid(row=0, column=0, sticky="nsew")
        vsb.grid(row=0, column=1, sticky="ns")
        hsb.grid(row=1, column=0, sticky="ew")
        mid.grid_rowconfigure(0, weight=1)
        mid.grid_columnconfigure(0, weight=1)

        # bottom buttons
        bottom = tk.Frame(self.root)
        bottom.pack(fill="x", padx=8, pady=6)

        tk.Button(bottom, text="Select All", command=self.select_all).pack(side="left", padx=4)
        tk.Button(bottom, text="Deselect All", command=self.deselect_all).pack(side="left", padx=4)
        tk.Button(bottom, text="Delete Selected (Move to CleanShelfRecycle)", command=self.delete_selected).pack(side="right", padx=4)
        tk.Button(bottom, text="Purge Old Recycle Files", command=lambda: purge_old_recycle_files_gui(show_message=True)).pack(side="right", padx=4)
        tk.Button(bottom, text="Refresh", command=self.refresh_current_listing).pack(side="right", padx=4)

        # status label
        self.status_var = tk.StringVar(value="Ready")
        self.status = tk.Label(self.root, textvariable=self.status_var, anchor="w")
        self.status.pack(fill="x", padx=8, pady=(0,6))

    def browse_folder(self):
        folder = filedialog.askdirectory(title="Select folder to scan")
        if folder:
            self.folder_path.set(folder)
            self.scan_and_populate()

    def scan_and_populate(self):
        folder = self.folder_path.get().strip()
        if not folder:
            messagebox.showwarning("No folder", "Please select a folder to scan.")
            return
        if not os.path.isdir(folder):
            messagebox.showerror("Invalid folder", "The selected path is not a folder or does not exist.")
            return

        self.status_var.set("Scanning...")
        self.root.update_idletasks()
        items = scan_folder_for_unused(folder, DAYS_UNUSED)
        # clear table
        for row in self.tree.get_children():
            self.tree.delete(row)
        # insert rows
        for name, last_access_str, fullpath in items:
            self.tree.insert("", "end", values=(name, last_access_str, fullpath))
        self.status_var.set(f"Found {len(items)} unused file(s) in {folder}")

    def refresh_current_listing(self):
        # re-scan the same folder if present
        folder = self.folder_path.get().strip()
        if folder:
            self.scan_and_populate()
        else:
            self.status_var.set("Select a folder then click Scan.")

    def select_all(self):
        for iid in self.tree.get_children():
            self.tree.selection_add(iid)

    def deselect_all(self):
        for iid in self.tree.get_children():
            self.tree.selection_remove(iid)

    def delete_selected(self):
        sel = self.tree.selection()
        if not sel:
            messagebox.showinfo("No selection", "Please select file rows to delete (Ctrl+Click to multi-select).")
            return
        # collect paths
        paths = [self.tree.item(iid, "values")[2] for iid in sel]
        # confirm
        ok = messagebox.askyesno("Confirm", f"Move {len(paths)} file(s) to CleanShelfRecycle? (They will be removable later.)")
        if not ok:
            return
        # move and update
        moved = move_to_custom_recycle(paths)
        self.status_var.set(f"Moved {moved} file(s) to CleanShelfRecycle.")
        # remove moved rows from table
        for iid in sel:
            self.tree.delete(iid)

# ------------- Run app -------------
if __name__ == "__main__":
    root = tk.Tk()
    app = CleanShelfApp(root)
    root.mainloop()
