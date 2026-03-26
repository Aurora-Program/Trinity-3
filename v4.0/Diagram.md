
classDiagram
class TritValor {
  <<enumeration>>
  CERO
  UNO
  DOS
}

class Rol {
  <<enumeration>>
  ES
  FN
  FO
}

class Estado {
  <<enumeration>>
  AMBIGUO
  COHERENTE
  INCOHERENTE
}

class TritDominio {
  +allowed : bitset<3>
}

class Operaciones {
  <<enumeration>>
  Or3
  And3
  Consensus3
}

class Trit {
  -dominio : TritDominio
  +ApplyEvent(const TritEvento& e) void
  +GetDato() optional<TritValor>
  +GetDominio() TritDominio
  +GetEstado() Estado
}

class TritConRol {
  -rol : Rol
  -trit : Trit*
  +GetRol() Rol
  +GetTrit() Trit*
}

class Dimension {
  -trits : TritConRol[3]
  -ordenador : TritValor
  +ApplyEvent(const DimensionEvento& e) void
  +GetTritsConRol() TritConRol[3]
  +Valido() bool
  +Reordenar() void
}

class Reloj {
  +Avanzar() void
}

Reloj --|> Dimension

class DimensionConRol {
  -dimension : Dimension*
  -rol : Rol
  +GetRol() Rol
  +GetDimension() Dimension*
}

class Vector {
  -dimensiones : DimensionConRol[3]
  -reloj : Reloj
  +GetDimensiones() DimensionConRol&[3]
  +ApplyEvent(const VectorEvento& e) void
}

class Trigate {
    -A : Trit*   # Entrada A
    -B : Trit*   # Entrada B
    -R : Trit*   # Resultado
    -M : Trit*   # Modo lógico
    -C : Trit*   # Modo operativo
    -E : Trit*   # Estado operativo
    +Cargar() void
    +ApplyEvent(TrigateEvent& e) void
    +Resolver() void
}

class TrigatePair {
    -form : Trigate
    -function : Trigate
    -E : Trit*
    -C : Trit*
    +Cargar() void
    +ApplyEvent(TrigatePairEvent& e) void
    +Resolver() void
}

class Destilador {
    -vectorInferior : Vector*
    -pares : TrigatePair[3]
    -E : Trit*
    -C : Trit*
    -dimensionesDestiladas : Dimension*[3]
    +ApplyEvent(DestiladorEvent& e) void
    +Resolver() void
}

class FuncionEmergencia {
    -dimensionDestilada : Dimension*
    -rol : Rol
    -S : Trit* #selector logico
    -emergente : Trit* #Resultado de emeregencia
    +ApplyEvent(FuncionEmergenciaEvent& e) void
    +Resolver() void
    -C: TritValor*
}

class Cara {
    -destilador : Destilador*
    -emergencias : FuncionEmergencia*[3]
    -S : Dimension*
    -emergencia : Dimension*
    -C : Trit*
    -E : Trit*
    +ApplyEvent(CaraEvent& e) void
    +Resolver() void
    +TipoCara() int
}

class Transcender {
    -caras : cara*[6]
    +ApplyEvent(TipoEvent& e) void
    +Resolver() void
}




