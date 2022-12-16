
(use-modules (guix packages))
(use-modules (guix git-download))
(use-modules (gnu packages bdw-gc))
(use-modules (gnu packages glib))
(use-modules (gnu packages libedit))
(use-modules (gnu packages xml))
(use-modules (gnu packages pkg-config))
(use-modules (gnu packages python))
(use-modules (gnu packages readline))
(use-modules (guix gexp))
(use-modules (guix build-system gnu))

(define %source-dir (getcwd))

(define-public 6d
  (package
    (name "6d")
    (version "FIXME")
      (source (local-file %source-dir
                          #:recursive? #t
                          #:select? (git-predicate %source-dir)))
    (build-system gnu-build-system)
    (native-inputs
     (list pkg-config python-wrapper))
    (inputs
     (list libgc editline glib libxml2))
    (synopsis "5d")
    (description "FIXME")
    (home-page "FIXME")
    (license #f)))

6d
