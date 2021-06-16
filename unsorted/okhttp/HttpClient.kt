import okhttp3.HttpUrl
import okhttp3.Response

interface HttpClient {

    fun builder(baseUrl: String): HttpUrl.Builder

    suspend fun getRequest(url: String, headers: Set<Pair<String, String>> = emptySet()): Response

    suspend fun getRequest(url: String, value: String, headers: Set<Pair<String, String>> = emptySet()): Response

    suspend fun postRequest(url: String, content: String, headers: Set<Pair<String, String>> = emptySet()): Response

    suspend fun putRequest(url: String, content: String, headers: Set<Pair<String, String>> = emptySet()): Response

    suspend fun putRequestAsync(url: String, content: String, headers: Set<Pair<String, String>> = emptySet())
}
