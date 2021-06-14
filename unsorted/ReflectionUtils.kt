import io.github.classgraph.ClassGraph
import kotlin.reflect.KClass

object ReflectionUtils {

    private const val DEFAULT_PACKAGE = ""

    private fun checkPackages(vararg packages: String) =
        if (packages.isEmpty()) {
            arrayOf(DEFAULT_PACKAGE)
        } else {
            packages
        }

    fun <T : Any> getImplementingOf(interfaceClass: KClass<out Any>, vararg packages: String) =
        ClassGraph()
            .enableClassInfo()
            .whitelistPackages(*checkPackages(*packages))
            .scan()
            .getClassesImplementing(interfaceClass.qualifiedName)
            .asSequence()
            .filter { it.isStandardClass && !it.isAbstract }
            .map { it.loadClass().kotlin }
            .filterIsInstance<KClass<T>>()
            .toSet()

    fun <T : Any> getImplementingOf(interfaceClass: Class<out Any>, vararg packages: String): Set<Class<T>> =
        getImplementingOf<T>(interfaceClass.kotlin, *packages)
            .map { it.java }
            .toSet()

    fun <T : Annotation> findAnnotationFrom(annotation: KClass<out T>, startingType: KClass<out Any>): T? =
        TypeHierarchy.getSequence(startingType)
            .flatMap { it.annotations.asSequence() }
            .filter { annotation.isInstance(it) }
            .firstOrNull() as? T

    inline fun <reified T : Any> getImplementingOf(vararg packages: String): Set<KClass<T>> =
        getImplementingOf(T::class, *packages)

    inline fun <reified T : Annotation> findAnnotationFrom(startingType: KClass<out Any>): T? =
        findAnnotationFrom(T::class, startingType)

    private object TypeHierarchy {

        fun getSequence(startingType: KClass<out Any>): Sequence<KClass<out Any>> =
            generateSequence(startingType.java)
                .distinct()
                .map { it.kotlin }

        private fun generateSequence(startingType: Class<*>?): Sequence<Class<*>> =
            if (startingType == null) {
                emptySequence()
            } else {
                sequenceOf(startingType)
                    .plus(startingType.interfaces)
                    .plus(startingType.superclass)
                    .plus(
                        generateSequence(startingType.superclass)
                    )
                    .plus(
                        startingType.interfaces.flatMap { i ->
                            generateSequence(i).asIterable()
                        }
                    )
                    .filterNotNull()
                    .filter { it != Any::class.java }
            }
    }
}
