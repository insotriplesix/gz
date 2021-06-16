package ru.cft.aml.fm.servicesapiclient.http

import kotlinx.coroutines.ExperimentalCoroutinesApi
import kotlinx.coroutines.suspendCancellableCoroutine
import okhttp3.Call
import okhttp3.Callback
import okhttp3.HttpUrl
import okhttp3.HttpUrl.Companion.toHttpUrlOrNull
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody
import okhttp3.Response
import java.io.IOException
import kotlin.coroutines.resumeWithException

@ExperimentalCoroutinesApi
class OkHttpBasedClient(private val client: OkHttpClient) : HttpClient {

    private val jsonMediaType = "application/json; charset=utf-8".toMediaTypeOrNull()

    override fun builder(baseUrl: String): HttpUrl.Builder =
        baseUrl
            .toHttpUrlOrNull()
            ?.newBuilder()
            ?: throw Exception("Can't parse URL: $baseUrl")

    override suspend fun getRequest(url: String, headers: Set<Pair<String, String>>): Response {
        val request = Request.Builder()
            .url(url)
            .addHeaders(headers)
            .get()
            .build()

        return client.newCall(request).await()
    }

    override suspend fun getRequest(url: String, value: String, headers: Set<Pair<String, String>>): Response =
        getRequest("$url/$value")

    override suspend fun postRequest(url: String, content: String, headers: Set<Pair<String, String>>): Response {
        val body = content.toRequestBody(jsonMediaType)
        val request = Request.Builder()
            .url(url)
            .addHeaders(headers)
            .post(body)
            .build()

        return client.newCall(request).await()
    }

    override suspend fun putRequest(url: String, content: String, headers: Set<Pair<String, String>>): Response {
        val body = content.toRequestBody(jsonMediaType)
        val request = Request.Builder()
            .url(url)
            .addHeaders(headers)
            .put(body)
            .build()

        return client.newCall(request).await()
    }

    override suspend fun putRequestAsync(url: String, content: String, headers: Set<Pair<String, String>>) {
        val body = content.toRequestBody(jsonMediaType)
        val request = Request.Builder()
            .url(url)
            .addHeaders(headers)
            .put(body)
            .build()

        client.newCall(request)
    }

    private suspend fun Call.await(recordStack: Boolean = false): Response {
        val callStack = if (recordStack) {
            IOException().apply {
                stackTrace = stackTrace.copyOfRange(1, stackTrace.size)
            }
        } else {
            null
        }

        return suspendCancellableCoroutine { continuation ->
            enqueue(
                object : Callback {
                    override fun onResponse(call: Call, response: Response) {
                        continuation.resume(response) { cause: Throwable -> continuation.cancel(cause) }
                    }

                    override fun onFailure(call: Call, e: IOException) {
                        if (continuation.isCancelled) return
                        callStack?.initCause(e)
                        continuation.resumeWithException(callStack ?: e)
                    }
                }
            )
            continuation.invokeOnCancellation {
                runCatching { cancel() }
            }
        }
    }

    private fun Request.Builder.addHeaders(headers: Set<Pair<String, String>>): Request.Builder {
        if (headers.isNotEmpty()) {
            headers.forEach { (name, value) ->
                this.addHeader(name, value)
            }
        }
        return this
    }
}
