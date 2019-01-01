;;; 50openscad.el -- debian emacs setups for openscad

(if (not (file-exists-p "/usr/share/emacs/site-lisp/scad-mode.el"))
    (message "openscad removed but not purged, skipping setup")

  ;; These as recommended by comments at the start of scad-mode.el, but \'
  ;; for the filename regexp.
  (autoload 'scad-mode "scad-mode" "Major mode for editing SCAD code." t)
  (add-to-list 'auto-mode-alist '("\\.scad\\'" . scad-mode)))
