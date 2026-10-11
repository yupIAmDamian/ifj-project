// Rozsireni VALMEM
import ifj26.*

@main def valmemExample(): Unit =
  val number = -3.75
  val truncated = number.toInt
  val floored = number.floor
  val floorInt = number.floor.toInt
  val digits = truncated.toString
  val length = "IFJ26".length
  val done: Unit = ()
  val unitText = done.toString
  print("truncated: ")
  print(digits)
  print("\nfloor: ")
  print(floored)
  print("\nfloor Int: ")
  print(floorInt)
  print("\nlength: ")
  print(length)
  print("\nUnit: ")
  print(unitText)
  print("\n")
