<!DOCTYPE STYLE-SHEET PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN"
  --
    This style sheet is an extension to the DocBook Modular Stylesheets.

    Copyright (C) 1999-2000 Frederik Fouvry

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    It has been taken over from the KDE style sheet of Ren� Beutler
    <rbeutler@g26.ethz.ch>, but many modifications were made.  Send
    suggestions, comments, etc. to Frederik Fouvry
    <fouvry@sfs.nphil.uni-tuebingen.de>.
  -- [
  <!ENTITY % loc-ents PUBLIC "-//KDE//ENTITIES KDE Localisation Style Sheet Entities//EN">
  %loc-ents;
  <!ENTITY dbmss-html PUBLIC "-//Norman Walsh//DOCUMENT DocBook HTML Stylesheet//EN" CDATA DSSSL>
  <!ENTITY dbmss-print PUBLIC "-//Norman Walsh//DOCUMENT DocBook Print Stylesheet//EN" CDATA DSSSL>
  <!ENTITY kde-localisation SYSTEM "kde-l10n.dsl" -- in namespace of language entities -->
  <!ENTITY kde-html-faq.dsl SYSTEM "kde-faq.dsl">
  <!ENTITY kde-html-navig.dsl SYSTEM "kde-navig.dsl">
  <!ENTITY kde-html-anchor.dsl SYSTEM "kde-anchor.dsl">
]>
<!--
    USAGE

    DSSSL

    There is information on how to call this dsl with jade in the
    style-specification-bodies.  Normally, the style sheets are identified
    with the following syntax: <DSSSL file>#<style-specification ID>,
    e.g. kde.dsl#kde-docbook-html-book.  The settings of the USEd
    style-specifications are inherited.

    For documentation of the functions and variables, and what you can
    do with the style sheets, see the DocBook Modular Style Sheets
    documentation.

    There are no provisions for customisation for specific documents.
    Should these be required, mail me on the above address.

    SGML

    Refer to this DOCUMENT as

      "-//KDE//DOCUMENT KDE Style Sheet V1.0//EN"

    For instance:

      <!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
        <!ENTITY kde.dsl PUBLIC "-//KDE//DOCUMENT KDE Style Sheet V1.0//EN">
      ]>

    and use kde.cat as the catalogue file.

    STRUCTURE
    
    KDE-DOCBOOK 		 uses KDE localisations
    KDE-DOCBOOK-PRINT 		 uses KDE-DOCBOOK DOCBOOK-PRINT
    KDE-DOCBOOK-TEX 		 uses KDE-DOCBOOK-PRINT
    KDE-DOCBOOK-TEX-BOOK 	 uses KDE-DOCBOOK-TEX
    KDE-DOCBOOK-HTML 		 uses KDE-DOCBOOK DOCBOOK-HTML
    KDE-DOCBOOK-HTML-BOOK 	 uses KDE-DOCBOOK-HTML
    KDE-DOCBOOK-HTML-ARTICLE	 uses KDE-DOCBOOK-HTML
    PRINT 			 uses KDE-DOCBOOK-TEX

    HTML 			 = KDE-DOCBOOK-HTML-BOOK
    TEX 			 = KDE-DOCBOOK-TEX
    DEFAULT-HTML 		 = DOCBOOK-HTML
    DEFAULT-PRINT 		 = DOCBOOK-PRINT

-->

<STYLE-SHEET>
  <STYLE-SPECIFICATION ID="KDE-DOCBOOK" USE="KDE-CA KDE-CS KDE-DA KDE-DE KDE-EL KDE-EN KDE-ES KDE-ET KDE-FI KDE-FR KDE-IT KDE-JA KDE-NL KDE-NO KDE-PL KDE-PT KDE-PT-BR KDE-RO KDE-RU KDE-SK KDE-CA KDE-SL KDE-SR KDE-SV KDE-ZH-CN">
    <STYLE-SPECIFICATION-BODY>
;; ===================================================================
;; KDE Generic Parameters
;; (Generic currently means: both print and html)
;; Call: jade -d kde.dsl#kde-docbook

;(define %default-language% (error "L10N ERROR: Use of the default language is not allowed!"))
; I feel these may be useful, but I have no clue what they do ;-)
(define %gentext-language% #f)
(define %gentext-use-xref-lang% #f)

; For language changes: if a A element is generated, it may be useful
; to add the language of the text referred to between brackets.

(define %kde-doc-common-path% "common/") ;; (relative) name of directory with common files

(define %linenumber-mod% 1)
(define %indent-programlisting-lines% " ")
(define %indent-screen-lines% " ")
(define %number-programlisting-lines% #t)
;(define %number-screen-lines% #t)      ;; should only be numbered in case of detailed discussion
(define %example-rules% #t)
(define %figure-rules% #t)
(define %table-rules% #t)
(define %equation-rules% #f)
(define %informalexample-rules% #t)
(define %informalfigure-rules% #f)
(define %informaltable-rules% #f)
(define %informalequation-rules% #f)
(define %chapter-autolabel% #t)
(define %section-autolabel% #t)
;(define (toc-depth nd) 3)


;; I have the feeling the next two should be removed (or
;; an error message should be issued).
;; Origin: print/dbprint.dsl
;; How:    replace "_" by "-"
;; Why:    "-" complies with RFC1766, "_" doesn't
;; Watch out: - if dsssl-language-code is redefined in the source
(define (dsssl-language-code #!optional (node (current-node)))
  (let* ((lang     ($lang$))
	 (langcode (if (> (string-index lang "-") 0)
		       (substring lang 0 (string-index lang "-"))
		       lang)))
    (string->symbol (case-fold-up langcode))))

;; Origin: print/dbprint.dsl
;; How:    replace "_" by "-"
;; Why:    "-" complies with RFC1766, "_" doesn't
;; Watch out: - if dsssl-country-code is redefined in the source
;; Note:   There are still problems with this: it seems to assume
;;         that there is only one "[-_]" in the code
(define (dsssl-country-code #!optional (node (current-node)))
  (let* ((lang     ($lang$))
	 (ctrycode (if (> (string-index lang "-") 0)
		       (substring lang
				  (+ (string-index lang "-") 1)
				  (string-length lang))
		       #f)))
    (if ctrycode
	(string->symbol (case-fold-up ctrycode))
	#f)))

;; Origin: common/dbl10n.dsl
;; How: replace "_" by "-"
;; Why: "-" complies with RFC1766, "_" doesn't
;; Watch out: - if lang-fix is redefined in the source
;; Note:   There are still problems with this: it seems to assume
;;         that there is only one "[-_]" in the code
(define (lang-fix language)
  (let ((fixed-lang (if (> (string-index language "_") 0)
			(let ((pos (string-index language "_")))
			  (string-append
			   (substring language 0 pos)
			   "-"
			   (substring language (+ pos 1)
				      (string-length language))))
			language)))
    (case-fold-down fixed-lang)))


;; Output encoding when Tex, such that the script can determine what to convert
;; a text to.

&kde-localisation;
    </STYLE-SPECIFICATION-BODY>
  </STYLE-SPECIFICATION>

  <STYLE-SPECIFICATION ID="KDE-DOCBOOK-PRINT" USE="KDE-DOCBOOK DOCBOOK-PRINT">
    <STYLE-SPECIFICATION-BODY>
;; ===================================================================
;; Print Parameters
;; printing isn't working very well yet
;; Call: jade -d kde.dsl#kde-docbook-print

; These are candidates to move to the general section
; Can also be set as -V image-library
(define image-library #f)
; This HAS to be in a directory different from the document
; with a catalogue that contains the ref to XML declaration
; possibly in the same dir as the pictures
; common/ is probably too general though ...
(define image-library-filename "images/imagelib.xml")

; -V tex-backend to activate tex backend
;(define default-backend 'tex)

;(define %mono-font-family% "\texttt")
(define %refentry-name-font-family% %mono-font-family%)
;(define %title-font-family% "\textsf")
;(define %body-font-family% "\textrm")
;(define %admon-font-family% "\textsf")
;(define %guilabel-font-family% "\textsf")

; Localised? - to country
(define %paper-type% "A4")

(define %hyphenation% #t)

   </STYLE-SPECIFICATION-BODY>
  </STYLE-SPECIFICATION>

  <STYLE-SPECIFICATION ID="KDE-DOCBOOK-TEX" USE="KDE-DOCBOOK-PRINT">
    <STYLE-SPECIFICATION-BODY>
;;======================================================================
;; Print Book parameters
;; Call: jade -d kde.dsl#kde-docbook-tex
(define tex-backend #t)

; These should not be put in the general style sheet, because
; defaults are different from HTML and other printing backends
(define %graphic-default-extension% "eps")
(define %graphic-extensions% (list "eps" "epsf"))
(define preferred-mediaobject-extensions (list "eps"))
(define acceptable-mediaobject-extensions '())
(define preferred-mediaobject-notations (list  "EPS"))
(define acceptable-mediaobject-notations (list "EPS" "linespecific"))

;; Adds the KDE logo to printed versions (thanks to �ric Bischoff)
;; From: print/dbcompon.dsl
;; It is probably too forceful, but it's a beginning
;; Here is also better than in a LaTeX style file, as it will also work for RTF
(define ($center-header$ #!optional (gi (gi)))
  (make external-graphic
    entity-system-id: "logotp3.eps"
    notation-system-id: "EPS"))

    </STYLE-SPECIFICATION-BODY>
  </STYLE-SPECIFICATION>

  <STYLE-SPECIFICATION ID="KDE-DOCBOOK-TEX-BOOK" USE="KDE-DOCBOOK-TEX">
    <STYLE-SPECIFICATION-BODY>
;;======================================================================
;; Print Book parameters
;; Call: jade -d kde.dsl#kde-docbook-tex-book

    </STYLE-SPECIFICATION-BODY>
  </STYLE-SPECIFICATION>

  <STYLE-SPECIFICATION ID="KDE-DOCBOOK-HTML" USE="KDE-DOCBOOK DOCBOOK-HTML">
    <STYLE-SPECIFICATION-BODY>
;; ===================================================================
;; HTML Parameters
;; user configurable settings in the html style sheet
;; Call: jade -d kde.dsl#kde-docbook-html

;; Any departure from a file that is not dbparam.dsl, should be
;; *extensively* documented, so that from reading the ChangeLog in the
;; DocBook modular style sheets, it can be determined what should change in
;; the KDE style files.

; === File names ===
; could this become a function?
(define %root-filename% "index")	;; name for the root html file
(define %html-ext% ".html")		;; default extension for html output files
(define %html-prefix% "")               ;; prefix for all filenames generated (except root)
(define %use-id-as-filename% #t)        ;; uses ID value, if present, as filename
                                        ;;   otherwise a code is used to indicate level
                                        ;;   of chunk, and general element number
                                        ;;   (nth element in the document)
(define use-output-dir #f)              ;; output in separate directory?
(define %output-dir% "HTML")            ;; if output in directory, it's called HTML

(define %graphic-default-extension% "png")
(define %graphic-extensions%            ;; default value + png - gif
  '("jpg" "jpeg" "tif" "tiff" "eps" "epsf" "png"))
(define preferred-mediaobject-extensions
  (list "png" "jpg" "jpeg"))
(define acceptable-mediaobject-extensions         ;; default value - gif + preferred
  (list "bmp" "eps" "epsf" "avi" "mpg" "mpeg" "qt"))
(define preferred-mediaobject-notations
  (list "PNG" "JPG" "JPEG"))
(define acceptable-mediaobject-notations
  (list "EPS" "BMP" "linespecific")) ;; default value - gif + preferred

(define %html-use-lang-in-filename% #f) ;; But could be very useful for l10n!
                                        ;; And HTTP Content-negotiation ...
                                        ;; I think it should become #t

; === HTML settings ===
(define %html-pubid% "-//W3C//DTD HTML 4.01 Transitional//EN") ;; Nearly true :-(
; ?? "-//Norman Walsh//DTD DocBook HTML 1.0//EN"
(define %html40% #t)

;; Origin: html/dbhtml.dsl
;; How:    replaced %html-header-tags%
;;         by (append %html-header-tags% (kde-gentext-html-header-tags))
;;         added META KEYWORDS element (with comma-separated keywords),
;;         derived from the original META KEYWORD element, which is still left in.
;; Why:    keeps any default values
;;         not sure search engines like many KEYWORD elements ...
;; Watch out: - if %html-header-tags% is used elsewhere
;;            - if $user-html-header$ is redefined
;;            - if the keyword calculation changes
;;            - the order of %html-header-tags% and kde-gentext-html-header-tags
;;              is important (see HTML specification)!
(define ($user-html-header$ #!optional
			    (home (empty-node-list))
			    (up (empty-node-list))
			    (prev (empty-node-list))
			    (next (empty-node-list)))
  (make sequence
    (let loop ((tl (append %html-header-tags%
			   (kde-gentext-html-header-tags)))) ; == modified here ==
      (if (null? tl)
	  (empty-sosofo)
	  (make sequence
	    (make empty-element gi: (car (car tl))
		attributes: (cdr (car tl)))
	    (loop (cdr tl)))))
    ; == a hack, but keeps localisation code cleaner
    ; == tests for RFC1766 compliance of language code
    ; == http://www.rfc-editor.org/rfc/rfc1766.txt
    ; == could go in the language-fix function
    (case ($lang$)
      (("no_ny") (error "L10N ERROR: use no-ny instead of no_ny")) ; ny is not a country but a language variant
      (("pt_br") (error "L10N ERROR: use pt-BR instead of pt_BR"))
      ; the versions with the encodings are possible, but should be written like
      ; this zh-CN-gb2312 and zh-TW-big5 (language info is lower case, country is upper case - none of this case sensitivity is compulsory: it's just convention
      ; unregistered languages must be written as x-LL
      (("zh_cn") (error "L10N ERROR: use zh-CN instead of zh_CN"))
      (("zh_cn.gb2312") (error "L10N ERROR: use zh-CN instead of zh_CN.GB2312"))
      (("zh-cn.gb2312") (error "L10N ERROR: use zh-CN instead of zh-CN.GB2312"))
      (("zh_tw") (error "L10N ERROR: use zh-TW instead of zh_TW"))
      (("zh_tw.big5") (error "L10N ERROR: use zh-TW instead of zh_TW.BIG5"))
      (("zh-tw.big5") (error "L10N ERROR: use zh-TW instead of zh-TW.BIG5"))
      (else (empty-sosofo)))
    ; == derived from dbhtml.dsl $standard-html-headers$
    (let ((nl (select-elements (descendants (info-element))
			       (normalize "keyword"))))
      (if (node-list-empty? nl)
	  (empty-sosofo)
	  (make empty-element gi: "META"
		attributes:
		(list (list "NAME" "KEYWORDS")
		      (list "CONTENT"
			    (string-append
			     (data (node-list-first nl))
			     (let loop ((nl1 (node-list-rest nl)))
			       (if (node-list-empty? nl1)
				   ""
				   (string-append ", "
						  (data (node-list-first nl1))
						  (loop (node-list-rest nl1)))))))))))
    ))

(define (kde-external-ss href #!optional (title #f)
			                 (rel "stylesheet")
					 (type "text/css"))
  (append (list "LINK" (list "REL" rel) (list "HREF" href) (list "TYPE" type))
	  (if title (list (list "TITLE" title)) '())))


(define %html-header-tags%        ;; for *general* KDE values only
;  '((META ("HTTP-EQUIV" "Content-Type") ("CONTENT" "text/html; charset=utf-8"))))
  (list
   ; make CSS default style sheet type
   '("META" ("HTTP-EQUIV" "Content-Style-Type") ("CONTENT" "text/css"))
    ; for KDE-wide style sheet
    (kde-external-ss (string-append %kde-doc-common-path% "kde-common.css"))
    ))
; Currently: language values are filled in by localisation
; kde-external-ss replaces DBMSS variable %stylesheet%

; image library: to be made by documentation authors (imagelib.xml)
; One with every document with pictures

(define %body-attr%
  (list
   (list "BGCOLOR" "#FFFFFF")
   (list "TEXT" "#000000")
   (list "LINK" "#aa0000")
   (list "STYLE" "font-family: sans-serif;"))) ;; This should go in CSS style sheets as well - it used to be Helvetica, Arial, sans-serif
(define %shade-verbatim% #t)
(define ($shade-verbatim-attr$)
  (list
   (list "BORDER" "0")
   (list "BGCOLOR" "#E0E0E0")
   (list "WIDTH" ($table-width$))))

; Doesn't really seem to work very well ...
(define %fix-para-wrappers% #f)
(define %spacing-paras% #f)             ;; to fix some vertical spacing,
                                        ;; NW says it's ugly

(define %css-decoration% #t)
(define %css-liststyle-alist%
  ;; Map DocBook OVERRIDE and MARK attributes to CSS
  '(("bullet" "disc")
    ("box" "square")))

; === Rendering ===
(define %admon-graphics% #t)	  ;; use symbols for Caution|Important|Note|Tip|Warning
(define %admon-graphics-path% (string-append %kde-doc-common-path%))
; Possibly not necessary to make so elaborate: the png part is added automatically!
; (define ($admon-graphic$ #!optional (nd (current-node)))
;   (cond ((equal? (gi nd) (normalize "tip"))
; 	 (string-append %admon-graphics-path% "tip.png"))
; 	((equal? (gi nd) (normalize "note"))
; 	 (string-append %admon-graphics-path% "note.png"))
; 	((equal? (gi nd) (normalize "important"))
; 	 (string-append %admon-graphics-path% "important.png"))
; 	((equal? (gi nd) (normalize "caution"))
; 	 (string-append %admon-graphics-path% "caution.png"))
; 	((equal? (gi nd) (normalize "warning"))
; 	 (string-append %admon-graphics-path% "warning.png"))
; 	(else (error (string-append (gi nd) " is not an admonition.")))))

(define ($admon-graphic$ #!optional (nd (current-node)))
  (cond ((or (equal? (gi nd) (normalize "tip"))
	     (equal? (gi nd) (normalize "note"))
	     (equal? (gi nd) (normalize "important"))
	     (equal? (gi nd) (normalize "caution"))
	     (equal? (gi nd) (normalize "warning")))
	 (string-append %admon-graphics-path%
			(case-fold-down (gi nd))
			"." %graphic-default-extension%))
	(else (error (string-append (gi nd) " is not an admonition.")))))

(define html-manifest #f)

;(define %default-quadding% "justify")            ;; Should be done in CSS (says NW)

; === Document structure ===
; See also the other specification below.

;(define %generate-legalnotice-link% #t) ;; works only if COPYRIGHT is present as well
;(define ($legalnotice-link-file$ legalnotice)
;  "http://www.kde.org/licence/gpl.html") ; works only if there are no other legal notices around!
; OTHERCREDIT can work, it just hasn't been done properly (see dbttlpg.dsl
; under articles): all that needs to be done is treating it as author;
; CONTRIB does not work therefore, but that also needs an extension
(define html-index #f) ; #f is default - #t generates the file html-index-filename
                       ; currently HTML.index; can be set to #t on the command line
;NO!;(define html-index-filename "realindex.docbook")
(define rootchunk nochunks)             ;; send output always to a file, not to stdout

; === Navigation ===
(define %header-navigation% #f)
(define %footer-navigation% #f)
(define %gentext-nav-use-tables% #f)
(define %gentext-nav-tblwidth% "50%") ; only effective if %gentext-nav-use-tables% is #t

&kde-html-navig.dsl;

;; Creates file with list of anchors
&kde-html-anchor.dsl;

    </STYLE-SPECIFICATION-BODY>
  </STYLE-SPECIFICATION>

  <STYLE-SPECIFICATION ID="KDE-DOCBOOK-HTML-BOOK" USE="KDE-DOCBOOK-HTML">
    <STYLE-SPECIFICATION-BODY>
;; ===================================================================
;; HTML Book Parameters
;; Call: jade -d kde.dsl#kde-docbook-html-book

(define %generate-book-titlepage% #t)
(define %generate-book-toc% #t)
(define ($generate-chapter-toc$) #f) ;; never generate a chapter TOC in books
                                     ;; comment out if unwanted and document

;; Thanks to David Mason (Gnome Documentation Project) for pointing out this variable
;; Origin: html/dbttlpg.dsl
;; How:    Added values we want in KDE: authorblurb, releaseinfo, date/pubdate,
;;         no revhistory
;;         Not prepended because we want to keep %titlepage-in-info-order% #f
;; Why:    to get author non-institutional e-mail address (in authorblurb)
;;         NOTE: authorblurb will be replaced in DocBook 4.0,
;; Watch out: - if (define (book-titlepage-recto-elements) ...) is modified
;;              in html/docbook.dsl
;;            - %title-page-in-info-order% is modified (dbparam.dsl)
(define (book-titlepage-recto-elements)
  (list
   (normalize "title")
   (normalize "subtitle")
   (normalize "graphic")
   (normalize "releaseinfo") ;; DB: about doc/software
   ;(normalize "edition")    ;; ~ to releaseinfo
   (normalize "date")
   (normalize "pubdate")     ;; Gnome is using this ... - not necessarily the same as date
   (normalize "corpauthor")
   (normalize "authorgroup")
   (normalize "author")
   (normalize "authorblurb") ; does not seem to work ... :-( - may not even need to be mentioned here, as it is part of whatever is in the author/editor section
   (normalize "editor")
   (normalize "copyright")
   (normalize "legalnotice")
   (normalize "abstract")
   ;(normalize "revhistory") ; probably we don't want this - or as verso element?
   ;(normalize "printhistory")
   ))

    </STYLE-SPECIFICATION-BODY>
  </STYLE-SPECIFICATION>

  <STYLE-SPECIFICATION ID="KDE-DOCBOOK-HTML-ARTICLE" USE="KDE-DOCBOOK-HTML">
    <STYLE-SPECIFICATION-BODY>
;; ===================================================================
;; HTML Article Parameters
;; Call: jade -d kde.dsl#kde-docbook-html-article

(define %generate-article-titlepage% #t)
(define %generate-article-toc% #t)      ;; make TOC

;; Contains KDE specific (re)definitions for QandA sets
; this and (element qandaset ...) (?) may need fine-tuning
; eg for numbering etc.
; Currently limited to articles
; These entities happen to contain only the qanda code - that may change
; to consider what will happen then (we don't always want qandasets to behave like this, or do we?)
&kde-html-faq.dsl;

; only for meant for long faqs!
(define %qanda-inherit-numeration%
  ;; PURP Should numbered questions inherit the surrounding numeration?
  ;; If true, question numbers are prefixed with the surrounding
  ;; component or section number. Has no effect unless
  ;; '%section-autolabel%' is also true.
  #t) ; = default

    </STYLE-SPECIFICATION-BODY>
  </STYLE-SPECIFICATION>

  <STYLE-SPECIFICATION ID="HTML" USE="KDE-DOCBOOK-HTML-BOOK">
    <STYLE-SPECIFICATION-BODY>
;;======================================================================
;; Makes #html an alias for #kde-docbook-html-book (for use with Cygnus)
;; Call: jade -d kde.dsl#html
;; Keep body empty
    </STYLE-SPECIFICATION-BODY>
  </STYLE-SPECIFICATION>

  <STYLE-SPECIFICATION ID="PRINT" USE="KDE-DOCBOOK-TEX">
    <STYLE-SPECIFICATION-BODY>
;;======================================================================
;; Makes #print an alias for #kde-docbook-print (for use with Cygnus)
;; Call: jade -d kde.dsl#print
;; Keep body empty
    </STYLE-SPECIFICATION-BODY>
  </STYLE-SPECIFICATION>

  <STYLE-SPECIFICATION ID="TEX" USE="KDE-DOCBOOK-TEX">
    <STYLE-SPECIFICATION-BODY>
;;======================================================================
;; Makes #print an alias for #kde-docbook-tex
;; Call: jade -d kde.dsl#tex

    </STYLE-SPECIFICATION-BODY>
  </STYLE-SPECIFICATION>

  <STYLE-SPECIFICATION ID="DEFAULT-HTML" USE="DOCBOOK-HTML">
    <STYLE-SPECIFICATION-BODY>
;;======================================================================
;; Calls the uncustomised DocBook Modular Style Sheet for HTML
;; Call: jade -d kde.dsl#default-html
;; Keep body empty
    </STYLE-SPECIFICATION-BODY>
  </STYLE-SPECIFICATION>

  <STYLE-SPECIFICATION ID="DEFAULT-PRINT" USE="DOCBOOK-PRINT">
    <STYLE-SPECIFICATION-BODY>
;;======================================================================
;; Calls the uncustomised DocBook Modular Style Sheet for print
;; Call: jade -d kde.dsl#default-print
;; Keep body empty
    </STYLE-SPECIFICATION-BODY>
  </STYLE-SPECIFICATION>

  <EXTERNAL-SPECIFICATION ID="DOCBOOK-HTML" DOCUMENT="dbmss-html">
  <EXTERNAL-SPECIFICATION ID="DOCBOOK-PRINT" DOCUMENT="dbmss-print">
  <EXTERNAL-SPECIFICATION ID="KDE-CA" DOCUMENT="kde-l1ca">
  <EXTERNAL-SPECIFICATION ID="KDE-CS" DOCUMENT="kde-l1cs">
  <EXTERNAL-SPECIFICATION ID="KDE-DA" DOCUMENT="kde-l1da">
  <EXTERNAL-SPECIFICATION ID="KDE-DE" DOCUMENT="kde-l1de">
  <EXTERNAL-SPECIFICATION ID="KDE-EL" DOCUMENT="kde-l1el">
  <EXTERNAL-SPECIFICATION ID="KDE-EN" DOCUMENT="kde-l1en">
  <EXTERNAL-SPECIFICATION ID="KDE-ES" DOCUMENT="kde-l1es">
  <EXTERNAL-SPECIFICATION ID="KDE-ET" DOCUMENT="kde-l1et">
  <EXTERNAL-SPECIFICATION ID="KDE-FI" DOCUMENT="kde-l1fi">
  <EXTERNAL-SPECIFICATION ID="KDE-FR" DOCUMENT="kde-l1fr">
  <EXTERNAL-SPECIFICATION ID="KDE-IT" DOCUMENT="kde-l1it">
  <EXTERNAL-SPECIFICATION ID="KDE-JA" DOCUMENT="kde-l1ja">
  <EXTERNAL-SPECIFICATION ID="KDE-NL" DOCUMENT="kde-l1nl">
  <EXTERNAL-SPECIFICATION ID="KDE-NO" DOCUMENT="kde-l1no">
  <EXTERNAL-SPECIFICATION ID="KDE-PL" DOCUMENT="kde-l1pl">
  <EXTERNAL-SPECIFICATION ID="KDE-PT" DOCUMENT="kde-l1pt">
  <EXTERNAL-SPECIFICATION ID="KDE-PT-BR" DOCUMENT="kde-l1pt-BR">
  <EXTERNAL-SPECIFICATION ID="KDE-RO" DOCUMENT="kde-l1ro">
  <EXTERNAL-SPECIFICATION ID="KDE-RU" DOCUMENT="kde-l1ru">
  <EXTERNAL-SPECIFICATION ID="KDE-SK" DOCUMENT="kde-l1sk">
  <EXTERNAL-SPECIFICATION ID="KDE-SL" DOCUMENT="kde-l1sl">
  <EXTERNAL-SPECIFICATION ID="KDE-SR" DOCUMENT="kde-l1sr">
  <EXTERNAL-SPECIFICATION ID="KDE-SV" DOCUMENT="kde-l1sv">
  <EXTERNAL-SPECIFICATION ID="KDE-ZH-CN" DOCUMENT="kde-l1zh-CN">

</STYLE-SHEET>
