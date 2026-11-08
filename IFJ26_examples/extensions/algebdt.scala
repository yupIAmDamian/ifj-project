// Rozsireni ALGEBDT
import ifj26.*

case class Point(x: Int, y: Int)  // soucinovy typ
type Input = Int | Unit | Point  // sjednoceni typu
type Amount = Double | Unit

def describe(value: Input): String =
  value match  // vyraz 'match'
    case n: Int if n > 0 => "positive integer"  // vzor overujici typ a podminku
    case 0 => "zero"  // vzor overujici literal
    case Point(x, y) =>  // vzor overujici soucinovy typ
      val sum = x + y
      val text = toStr(sum)
      "point sum: " + text  // posledni radek je vysledek vetve
    case unit: Unit => "invalid input"  // vzor overujici pouze typ
    case _ => "negative integer"  // vychozi vetev

def describeAmount(value: Amount): String =
  value match
    case n: Double =>
      val text = toStr(n)
      "amount: " + text
    case _ => "invalid amount"

@main def algebdtExample(): Unit =
  val input: Input = readIntMaybe()
  val first = describe(input)
  val point = Point(2, 3)
  val second = describe(point)
  val amount: Amount = readDoubleMaybe()
  val third = describeAmount(amount)
  print(first)
  print("\n")
  print(second)
  print("\n")
  print(third)
  print("\n")
