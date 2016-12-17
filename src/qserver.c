/*
 * Q server for R
 */

/*
 * The public interface used from R.
 */

#ifdef WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

EXPORT SEXP kx_r_open_connection(SEXP);
EXPORT SEXP kx_r_close_connection(SEXP);
EXPORT SEXP kx_r_execute(SEXP c, SEXP, SEXP);
EXPORT SEXP kx_r_convert_binary(SEXP);
EXPORT SEXP kx_r_convert_r(SEXP);

/*
 * Open a connection to an existing kdb+ process.
 *
 * If we just have a host and port we call khp from the kdb+ interface.
 * If we have a host, port, "username:password" we call instead khpu.
 */
SEXP kx_r_open_connection(SEXP whence)
{
	SEXP result;
	int connection, port;
	char *host;
	int length = GET_LENGTH(whence);
	if (length < 2)
		error("Can't connect with so few parameters..");

	port = INTEGER_POINTER (VECTOR_ELT(whence, 1))[0];
	host = (char*) CHARACTER_VALUE(VECTOR_ELT(whence, 0));

	if (2 == length)
		connection = khp(host, port);
	else {
		char *user = (char*) CHARACTER_VALUE(VECTOR_ELT (whence, 2));
		connection = khpu(host, port, user);
	}
        if (!connection)
          error("Could not authenticate");
        else if (connection < 0) {
#ifdef WIN32
          char buf[256];
          FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
          error(buf);
#else
	  error(strerror(errno));
#endif
	}
	PROTECT(result = NEW_INTEGER(1));
	INTEGER_POINTER(result)[0] = connection;
	UNPROTECT(1);
	return result;
}

/*
 * Close a connection to an existing kdb+ process.
 */
SEXP kx_r_close_connection(SEXP connection)
{
	SEXP result;

	/* Close the connection. */
	kclose(INTEGER_VALUE(connection));

	PROTECT(result = NEW_INTEGER(1));
	INTEGER_POINTER(result)[0] = 0;
	UNPROTECT(1);
	return result;
}

/*
 * Execute a kdb+ query over the given connection.
 */
SEXP kx_r_execute(SEXP connection, SEXP query, SEXP args) {
	K result;
	SEXP s;
	char *query_str;
	kx_connection = INTEGER_VALUE(connection);
	size_t nargs = LENGTH(args);

	if (nargs > 8) {
		error("Error: kdb+ functions take a maximum of 8 parameters");
	}
	if(TYPEOF(query) != STRSXP) {
		error("Error: supplied query or function name must be a string");
	}
	query_str = (char*) CHARACTER_VALUE(query);
	K kargs[8] = { (K)0 };
	for (size_t i = 0; i < nargs; i++) {
		kargs[i] = from_any_robject(VECTOR_ELT(args, i));
	}

	result = k(kx_connection, query_str, kargs[0], kargs[1], kargs[2],
				kargs[3], kargs[4], kargs[5], kargs[6], kargs[7], (K)0);

	if (0 == result) {
		error("Error: not connected to kdb+ server\n");
	}
	else if (kx_connection < 0) { //async IPC
		return R_NilValue;
	}
	else if (-128 == result->t) {
		char *e = calloc(strlen(result->s) + 1, 1);
		strcpy(e, result->s);
		r0(result);
		error("Error from kdb+: `%s\n", e);
	}
	s = from_any_kobject(result);
	r0(result);
	return s;
}

SEXP kx_r_convert_binary(SEXP binary)
{
	K result, bytes;
	SEXP s;
	int n = length(binary);
	unsigned char arr[n];

	for (int i = 0; i < n; i++) {arr[i] = RAW(binary)[i];}

	khp("",-1);
	bytes = ktn(KG, sizeof(arr));
	memcpy(kG(bytes), arr, sizeof(arr));

	if(okx(bytes) & n > 1) {
		result = d9(bytes);
		s = from_any_kobject(result);
		r0(result);
		r0(bytes);
	} else {
		r0(bytes);
		// error("raw value is not a valid kdb data type");
		PROTECT(s = NEW_INTEGER(1));
		INTEGER_POINTER(s)[0] = 0;
		UNPROTECT(1);
	}
	return s;
}

SEXP kx_r_convert_r(SEXP robj)
{
	K conversion, serialized;
	SEXP result;

	khp("", -1);
	conversion = from_any_robject(robj);
	serialized = b9(2, conversion);

	PROTECT(result = allocVector(RAWSXP, serialized->n));
	for(int i = 0; i < length(result); i++) {RAW(result)[i] = (unsigned char) kG(serialized)[i];}

	r0(conversion);
	r0(serialized);
	UNPROTECT(1);

	return result;
}
