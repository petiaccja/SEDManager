class CachedStream<T> {
  CachedStream(this.source);

  final Stream<T> source;
  T? latest;

  late final stream = source.map((event) {
    latest = event;
    return event;
  });
}