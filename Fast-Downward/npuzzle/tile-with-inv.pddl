
;; The sliding-tile puzzle with invariants.

(define (domain strips-sliding-tile)
  (:requirements :strips)
  (:predicates
   (tile ?x) (position ?x)
   (at ?t ?x ?y) (blank ?x ?y)
   (inc ?p ?pp) (dec ?p ?pp))

  (:action move-up
    :parameters (?t ?px ?py ?by)
    :precondition (and
		   (tile ?t) (position ?px) (position ?py) (position ?by)
		   (dec ?by ?py) (blank ?px ?by) (at ?t ?px ?py))
    :effect (and (not (blank ?px ?by)) (not (at ?t ?px ?py))
		 (blank ?px ?py) (at ?t ?px ?by)))

  (:action move-down
    :parameters (?t ?px ?py ?by)
    :precondition (and
		   (tile ?t) (position ?px) (position ?py) (position ?by)
		   (inc ?by ?py) (blank ?px ?by) (at ?t ?px ?py))
    :effect (and (not (blank ?px ?by)) (not (at ?t ?px ?py))
		 (blank ?px ?py) (at ?t ?px ?by)))

  (:action move-left
    :parameters (?t ?px ?py ?bx)
    :precondition (and
		   (tile ?t) (position ?px) (position ?py) (position ?bx)
		   (dec ?bx ?px) (blank ?bx ?py) (at ?t ?px ?py))
    :effect (and (not (blank ?bx ?py)) (not (at ?t ?px ?py))
		 (blank ?px ?py) (at ?t ?bx ?py)))

  (:action move-right
    :parameters (?t ?px ?py ?bx)
    :precondition (and
		   (tile ?t) (position ?px) (position ?py) (position ?bx)
		   (inc ?bx ?px) (blank ?bx ?py) (at ?t ?px ?py))
    :effect (and (not (blank ?bx ?py)) (not (at ?t ?px ?py))
		 (blank ?px ?py) (at ?t ?bx ?py)))

  ;; position of tile ?t
  (:invariant
   :name loc
   :vars (?t)
   :context (tile ?t)
   :set-constraint
   (exactly-n 1 (setof :vars (?x ?y)
		       :context (and (position ?x) (position ?y))
		       (at ?t ?x ?y)))
   )

  ;; position of blank
  (:invariant
   :name loc_blank
   :set-constraint
   (exactly-n 1 (setof :vars (?x ?y)
		       :context (and (position ?x) (position ?y))
		       (blank ?x ?y)))
   )

  ;; content of square ?x ?y
  (:invariant
   :name content_of
   :vars (?x ?y)
   :context (and (position ?x) (position ?y))
   :set-constraint
   (exactly-n 1 (setof :vars (?t) :context (tile ?t) (at ?t ?x ?y))
	      (blank ?x ?y))
   )
  )
