// Rozsireni LISTS (v kombinaci s CONTROL i LAMBDAS+FUNEXP+BOOLTHEN)
import ifj26.*

@main def listsExample(): Unit =
  val values: List[Int] = (1 to 6).toList
  val joined = values ++ List(7, 8)
  val middle = joined.drop(1).take(5).slice(1, 4)
  val first = middle.head
  val rest = middle.tail
  val indexed = middle(1)
  val selected = joined.filter((value: Int) => value >= 3 && value <= 6)
  val prefix = joined.takeWhile((value: Int) => value < 4)
  val suffix = joined.dropWhile((value: Int) => value < 6)
  val empty: List[Int] = List()
  val descending = List.range(5, 0, -2)
  var sum = 0
  for
    value <- selected
    if value != 4
  do
    sum = sum + value
  print(middle.toString)
  print("\nhead: ")
  print(first)
  print("\nindex 1: ")
  print(indexed)
  print("\ntail: ")
  print(rest)
  print("\nfilter: ")
  print(selected)
  print("\ntakeWhile: ")
  print(prefix)
  print("\ndropWhile: ")
  print(suffix)
  print("\nempty: ")
  print(empty)
  print("\nrange: ")
  print(descending)
  print("\nsum: ")
  print(sum)
  print("\n")
