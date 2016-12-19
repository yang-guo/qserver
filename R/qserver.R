#' @useDynLib qserver

#' @export
open_connection <- function(host="localhost", port=5000, user=NULL) {
         parameters <- list(host, as.integer(port), user)
      h <- .Call("kx_r_open_connection", parameters)
    assign(".k.h", h, envir = .GlobalEnv)
    h
}

#' @export
execute <- function(connection, query, ...) {
  .Call("kx_r_execute", as.integer(connection), query, list(...))
}

#' @export
close_connection <- function(connection) {
	.Call("kx_r_close_connection", as.integer(connection))
}

#' @export
convert_binary <- function(raw_vector) {
	.Call("kx_r_convert_binary", raw_vector)
}

#' @export
convert_r <- function(r_obj) {
	.Call("kx_r_convert_r", r_obj)
}

#' @export
k <- function(query, ...) {
    h <- get(".k.h", envir = .GlobalEnv)
    execute(h, query, ...)
}
