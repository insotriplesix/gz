import com.fasterxml.jackson.databind.ObjectMapper
import com.hazelcast.core.ExecutionCallback
import com.hazelcast.core.ICompletableFuture
import kotlinx.coroutines.CancellableContinuation
import kotlinx.coroutines.ExperimentalCoroutinesApi
import kotlinx.coroutines.cancelFutureOnCancellation
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.collect
import kotlinx.coroutines.suspendCancellableCoroutine
import org.springframework.util.concurrent.ListenableFuture
import org.springframework.util.concurrent.ListenableFutureCallback
import java.util.concurrent.ExecutionException
import kotlin.coroutines.resumeWithException

@ExperimentalCoroutinesApi
suspend fun <T> ListenableFuture<T>.await(): T? =
    if (isDone) {
        try {
            get()
        } catch (e: ExecutionException) {
            throw e.cause ?: e
        }
    } else {
        suspendCancellableCoroutine { cont: CancellableContinuation<T?> ->
            addCallback(object : ListenableFutureCallback<T> {
                override fun onFailure(exception: Throwable) = cont.resumeWithException(exception)
                override fun onSuccess(result: T?) = cont.resume(result) { cancel(false) }
            })
            cont.cancelFutureOnCancellation(this)
        }
    }

@ExperimentalCoroutinesApi
suspend fun <T> ICompletableFuture<T>.await(): T? =
    if (isDone) {
        try {
            get()
        } catch (e: ExecutionException) {
            throw e.cause ?: e
        }
    } else {
        suspendCancellableCoroutine { cont: CancellableContinuation<T?> ->
            andThen(object : ExecutionCallback<T> {
                override fun onFailure(t: Throwable) = cont.resumeWithException(t)
                override fun onResponse(response: T?) = cont.resume(response) { cancel(false) }
            })
            cont.cancelFutureOnCancellation(this)
        }
    }

suspend inline fun <K, V> Flow<Pair<K, V>>.toMap(destination: MutableMap<K, V> = mutableMapOf()): Map<K, V> {
    collect { (key, value) ->
        destination[key] = value
    }
    return destination
}

inline fun <reified T> ObjectMapper.writeAsString(value: T): String =
    writeValueAsString(value)
