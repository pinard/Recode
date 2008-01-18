;;; utf8.el --- Insert UTF-8 encoded ISO-10646/Unicode characters.
;;;
;;; Copyright (C) 1994 Gary Houston <ghouston@actrix.gen.nz>
;;;
;;; This program is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 2 of the License, or
;;; (at your option) any later version.
;;;
;;; This program is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;;
;;; You should have received a copy of the GNU General Public License
;;; along with this program; if not, write to the Free Software
;;; Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

;;; COMMENTARY
;;; ----------

;;; Provides a command for direct entry of UTF-8 encoded multibyte
;;; characters in hexadecimal form.

;;; Load file and type `M-x hex-to-utf8'

;;; Also contains a simple symbol selection command (not many symbols):
;;; M-x utf8-symbol and TAB for completion.

;;; A list of Unicode names and codes is available by ftp from unicode.org.

;;; Modification of Emacs to display and process the multibyte characters
;;; correctly is left as an exercise for the reader.

;;; HISTORY
;;; -------
;;; Created 1994-10-24.  Experimental.  May contain bugs.
;;; Uses a few lines of code from hexl.el.

;;; THEORY
;;; ------
;;; The UTF-FSS (aka UTF-2 aka UTF-8) encoding of UCS, as described in the
;;; following quote from Ken Thompson's utf-fss.c:
;;;
;;;Bits  Hex Min  Hex Max  Byte Sequence in Binary
;;;  7  00000000 0000007f 0vvvvvvv
;;; 11  00000080 000007FF 110vvvvv 10vvvvvv
;;; 16  00000800 0000FFFF 1110vvvv 10vvvvvv 10vvvvvv
;;; 21  00010000 001FFFFF 11110vvv 10vvvvvv 10vvvvvv 10vvvvvv
;;; 26  00200000 03FFFFFF 111110vv 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv
;;; 31  04000000 7FFFFFFF 1111110v 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv
;;;
;;; The UCS value is just the concatenation of the v bits in the multibyte
;;; encoding.  When there are multiple ways to encode a value, for example
;;; UCS 0, only the shortest encoding is legal.

(defun hex-to-utf8 (hex-string)
  "Convert a hex string to a UTF-8 multibyte character."
  (interactive "sEnter hex character value (0 - 7FFFFFFF): ")
  (let ((widechar 0.0)
	(num-bytes 1)
	(orig-hex hex-string))
    ;; Convert the string to a decimal float.
    ;; Floats are used since elisp integers may be too small for
    ;; large characters.
    (while (not (equal hex-string ""))
      (setq widechar
	    (+ (* widechar 16)
	       (utf8-hex-char-to-integer (string-to-char hex-string))))
      (setq hex-string (substring hex-string 1)))
    (if (>= widechar (expt 2.0 31))
	(error (format "char value %s is larger than 7FFFFFFF" orig-hex)))

    ;; Calculate the number of UTF-8 bytes needed.
    (while (>= widechar
	       ;; 2 raised to the number of bits with this many bytes.
	       (expt
		2.0
		(+
		 (if (= num-bytes 1) 7 (- 7 num-bytes))
		 (* (- num-bytes 1) 6))))
      (setq num-bytes (+ num-bytes 1)))

    (insert
     ;; First byte.
     (char-to-string 
      (logior
       (truncate (/ widechar (expt 2.0 (* (- num-bytes 1) 6))))
       (if (= num-bytes 1)
	   0
	 (mod (lognot (- (expt 2 (+ 8 (- num-bytes))) 1)) 256)))))

    ;; Remaining bytes.
    (while (> num-bytes 1)
      (setq num-bytes (- num-bytes 1))
      (insert
       (char-to-string
	(logior
	 (truncate
	  (/
	   (mod widechar (expt 2.0 (* num-bytes 6)))
	   (expt 2.0 (* (- num-bytes 1) 6))))
	 128))))))
	  
(defun utf8-hex-char-to-integer (character)
  "Take a char and return its value as if it was a hex digit."
  (if (and (>= character ?0) (<= character ?9))
      (- character ?0)
    (let ((ch (logior character 32)))
      (if (and (>= ch ?a) (<= ch ?f))
	  (- ch (- ?a 10))
	(error (format "Invalid hex digit `%c'." ch))))))

;;; Symbol selection stuff.

(defconst ISO-10646-symbol-types
  '(("greek-basic" nil)
  ("chess" nil)
  ("cards" nil))
  "Classes of ISO-10646 symbols")

(defconst ISO-10646-greek-basic
  '(("GREEK CAPITAL LETTER ALPHA" "0391")
    ("GREEK CAPITAL LETTER BETA" "0392")
    ("GREEK CAPITAL LETTER GAMMA" "0393")
    ("GREEK CAPITAL LETTER DELTA" "0394")
    ("GREEK CAPITAL LETTER EPSILON" "0395")
    ("GREEK CAPITAL LETTER ZETA" "0396")
    ("GREEK CAPITAL LETTER ETA" "0397")
    ("GREEK CAPITAL LETTER THETA" "0398")
    ("GREEK CAPITAL LETTER IOTA" "0399")
    ("GREEK CAPITAL LETTER KAPPA" "039A")
    ("GREEK CAPITAL LETTER LAMDA" "039B")
    ("GREEK CAPITAL LETTER MU" "039C")
    ("GREEK CAPITAL LETTER NU" "039D")
    ("GREEK CAPITAL LETTER XI" "039E")
    ("GREEK CAPITAL LETTER OMICRON" "039F")
    ("GREEK CAPITAL LETTER PI" "03A0")
    ("GREEK CAPITAL LETTER RHO" "03A1")
    ("GREEK CAPITAL LETTER SIGMA" "03A3")
    ("GREEK CAPITAL LETTER TAU" "03A4")
    ("GREEK CAPITAL LETTER UPSILON" "03A5")
    ("GREEK CAPITAL LETTER PHI" "03A6")
    ("GREEK CAPITAL LETTER CHI" "03A7")
    ("GREEK CAPITAL LETTER PSI" "03A8")
    ("GREEK CAPITAL LETTER OMEGA" "03A9")
    ("GREEK SMALL LETTER ALPHA" "03B1")
    ("GREEK SMALL LETTER BETA" "03B2")
    ("GREEK SMALL LETTER GAMMA" "03B3")
    ("GREEK SMALL LETTER DELTA" "03B4")
    ("GREEK SMALL LETTER EPSILON" "03B5")
    ("GREEK SMALL LETTER ZETA" "03B6")
    ("GREEK SMALL LETTER ETA" "03B7")
    ("GREEK SMALL LETTER THETA" "03B8")
    ("GREEK SMALL LETTER IOTA" "03B9")
    ("GREEK SMALL LETTER KAPPA" "03BA")
    ("GREEK SMALL LETTER LAMDA" "03BB")
    ("GREEK SMALL LETTER MU" "03BC")
    ("GREEK SMALL LETTER NU" "03BD")
    ("GREEK SMALL LETTER XI" "03BE")
    ("GREEK SMALL LETTER OMICRON" "03BF")
    ("GREEK SMALL LETTER PI" "03C0")
    ("GREEK SMALL LETTER RHO" "03C1")
    ("GREEK SMALL LETTER FINAL SIGMA" "03C2")
    ("GREEK SMALL LETTER SIGMA" "03C3")
    ("GREEK SMALL LETTER TAU" "03C4")
    ("GREEK SMALL LETTER UPSILON" "03C5")
    ("GREEK SMALL LETTER PHI" "03C6")
    ("GREEK SMALL LETTER CHI" "03C7")
    ("GREEK SMALL LETTER PSI" "03C8")
    ("GREEK SMALL LETTER OMEGA" "03C9")
    ("GREEK BETA SYMBOL" "03D0")
    ("GREEK THETA SYMBOL" "03D1")
    ("GREEK PHI SYMBOL" "03D5")
    ("GREEK PI SYMBOL" "03D6")
    ("GREEK LETTER STIGMA" "03DA")
    ("GREEK LETTER DIGAMMA" "03DC")
    ("GREEK LETTER KOPPA" "03DE")
    ("GREEK LETTER SAMPI" "03E0")
    ("GREEK KAPPA SYMBOL" "03F0")
    ("GREEK RHO SYMBOL" "03F1")
    ("GREEK LUNATE SIGMA SYMBOL" "03F2")
    ("GREEK LETTER YOT" "03F3")
    ("TURNED GREEK SMALL LETTER IOTA" "2129"))
  "Names and character codes for ISO-10646 unaccented Greek letters.")

(defconst ISO-10646-chess
  '(("WHITE CHESS KING" "2654")
    ("WHITE CHESS QUEEN" "2655")
    ("WHITE CHESS ROOK" "2656")
    ("WHITE CHESS BISHOP" "2657")
    ("WHITE CHESS KNIGHT" "2658")
    ("WHITE CHESS PAWN" "2659")
    ("BLACK CHESS KING" "265A")
    ("BLACK CHESS QUEEN" "265B")
    ("BLACK CHESS ROOK" "265C")
    ("BLACK CHESS BISHOP" "265D")
    ("BLACK CHESS KNIGHT" "265E")
    ("BLACK CHESS PAWN" "265F"))
  "Names and character codes for ISO-10646 chess pieces.")

(defconst ISO-10646-cards
  '(("BLACK SPADE SUIT" "2660")
    ("WHITE HEART SUIT" "2661")
    ("WHITE DIAMOND SUIT" "2662")
    ("BLACK CLUB SUIT" "2663")
    ("WHITE SPADE SUIT" "2664")
    ("BLACK HEART SUIT" "2665")
    ("BLACK DIAMOND SUIT" "2666")
    ("WHITE CLUB SUIT" "2667"))
  "Names and character codes for ISO-10646 playing cards.")

(defun utf8-symbol ()
  "Insert a UTF-8 encoded symbol from ISO-10646."
  (interactive)
  (do-ISO-10646-symbol-type
   (completing-read "Symbol category: " ISO-10646-symbol-types nil t)))

(defun do-ISO-10646-symbol-type (category)
  "Insert a UTF-8 encoded symbol from a given category."
  (let ((category-var (intern (concat "ISO-10646-" category)))
    (completion-ignore-case t))
    (hex-to-utf8
     (elt
      (assoc
       (completing-read "Name of the symbol: "
			(symbol-value category-var) nil t)
       (symbol-value category-var)) 1))))
