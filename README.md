**Introduction**

In modern computing environments, continuous file creation, downloads, and application usage result in large volumes of unused and obsolete files accumulating over time. These unused files consume valuable storage space, reduce system efficiency, and make file management difficult. Manual identification and deletion of such files is time-consuming, error-prone, and may lead to accidental data loss.

CleanShelf is a Python-based desktop application designed to address this problem by automatically identifying unused files and providing a safe, controlled mechanism for cleaning them. Instead of permanently deleting files, CleanShelf introduces a custom recycle system that ensures data safety while optimizing storage usage.

**** Problem Statement***

Most operating systems provide basic cleanup utilities; however, they suffer from several limitations:

Lack of transparency in file selection

Immediate permanent deletion risks

No detailed logging of deleted files

Limited user control over retention policies

Users often hesitate to delete files because they are unsure whether the files may be needed in the future. As a result, storage clutter continues to grow, impacting system performance.

** Objectives of the System**

The primary objectives of the CleanShelf system are:

To automatically identify files that have not been accessed for a defined period.

To provide a graphical interface for easy user interaction.

To prevent accidental data loss through safe deletion mechanisms.

To maintain logs for transparency and audit purposes.

To improve system storage efficiency without compromising data safety.

4. System Overview

CleanShelf is implemented as a desktop application using Python. It uses file system metadata such as last accessed timestamps to determine file usage. The system follows a non-destructive cleanup approach by moving files to a custom recycle folder instead of deleting them permanently.

The application consists of multiple modules that work together to scan directories, display unused files, manage deletions, and perform automated cleanup of old recycle data.

5. Functional Description
5.1 File Scanning Mechanism

The system scans user-selected folders recursively and retrieves metadata for each file. A file is considered unused if its last access time exceeds a configurable threshold (for example, 60 days). This ensures that only genuinely inactive files are selected.

5.2 Graphical User Interface (GUI)

The GUI is developed using the Tkinter library and provides:

Folder selection capability

Tabular display of unused files

Information such as file name, last accessed date, and file path

Options to select, deselect, refresh, and delete files

This interface makes the application accessible even to non-technical users.

5.3 Safe Deletion Strategy

Instead of deleting files permanently, CleanShelf moves selected files to a custom directory called CleanShelfRecycle. This approach ensures:

Files can be manually restored if required

Accidental deletion is avoided

User confidence in cleanup operations

5.4 Deletion Logging System

Each deleted file is recorded in a JSON-based log file. The log contains:

File location

Date of deletion

This logging mechanism provides accountability and helps track cleanup operations.

5.5 Retention and Auto-Purge Mechanism

Files stored in the CleanShelfRecycle folder are retained for a configurable number of days. After this retention period expires, the system permanently deletes those files. This ensures that the recycle folder does not grow indefinitely while still providing recovery time.

6. System Architecture (Conceptual)

The CleanShelf system follows a modular architecture:

User Interface Module – Handles interaction with the user

File Analysis Module – Identifies unused files

Recycle Management Module – Handles safe deletion and restoration

Logging Module – Records cleanup activities

Automation Module – Handles scheduled or startup-based cleanup checks

This separation of concerns improves maintainability and scalability.

7. Technologies Used

Python – Core programming language

Tkinter – GUI development

OS & Shutil Modules – File and directory operations

JSON – Lightweight logging format

Datetime & Time Modules – File age calculation

Psutil / Send2Trash (Optional) – System and storage utilities

8. Configuration Parameters

The system allows easy modification of cleanup rules using predefined parameters such as:

Number of days after which a file is considered unused

Number of days files remain in the recycle folder before permanent deletion

This configurability makes CleanShelf adaptable to different user needs.

9. Advantages of the System

Prevents accidental permanent deletion

Improves storage utilization

Reduces manual cleanup effort

Provides transparency through logs

User-friendly and intuitive design

Applicable to real-world file management problems

10. Limitations

Currently designed for local file systems only

No cloud backup integration

Requires manual execution unless scheduled externally

11. Future Enhancements

Future versions of CleanShelf can include:

Background scheduled cleanup

Cloud-based backup before deletion

File-type specific cleanup rules

Advanced analytics and reports

Cross-platform packaged installers

12. Applications

Personal computers with limited storage

Academic and office systems

File-heavy environments such as media editing systems

Backup and archival management

13. Conclusion

CleanShelf provides an effective and safe solution for managing unused files and optimizing system storage. By combining automation with user control and safety mechanisms, the system bridges the gap between manual cleanup and risky permanent deletion. Its modular design, transparency, and practicality make it suitable for both academic projects and real-world applications.
