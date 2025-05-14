Vamos provar as duas identidades:

- V⊥Q−1=QV⊥V_{\perp} Q^{-1} = Q V_{\perp}
    
- V∥Q−1=Q−1V∥V_{\parallel} Q^{-1} = Q^{-1} V_{\parallel}
    

**Dica fornecida:** usar a decomposição vetorial v⃗=v⃗∥+v⃗⊥\vec{v} = \vec{v}_{\parallel} + \vec{v}_{\perp}, onde:

- v⃗∥\vec{v}_{\parallel} é **paralelo** ao eixo de rotação n⃗\vec{n}
    
- v⃗⊥\vec{v}_{\perp} é **ortogonal** a n⃗\vec{n}
    

## Passo 1: Produto de quatérnios puros

Do slide 40:

(0,u⃗)(0,v⃗)=(−⟨u⃗,v⃗⟩,u⃗×v⃗)(0, \vec{u})(0, \vec{v}) = (-\langle \vec{u}, \vec{v} \rangle, \vec{u} \times \vec{v})

**Mostre que:**

- v⃗⊥n⃗=−n⃗v⃗⊥\vec{v}_{\perp} \vec{n} = -\vec{n} \vec{v}_{\perp}
    
- v⃗∥n⃗=n⃗v⃗∥\vec{v}_{\parallel} \vec{n} = \vec{n} \vec{v}_{\parallel}
    

### A. v⃗⊥n⃗=−n⃗v⃗⊥\vec{v}_{\perp} \vec{n} = -\vec{n} \vec{v}_{\perp}

Como v⃗⊥⋅n⃗=0\vec{v}_{\perp} \cdot \vec{n} = 0, o produto de quatérnios puros é:

(0,v⃗⊥)(0,n⃗)=(0,v⃗⊥×n⃗)(0, \vec{v}_{\perp})(0, \vec{n}) = (0, \vec{v}_{\perp} \times \vec{n}) (0,n⃗)(0,v⃗⊥)=(0,n⃗×v⃗⊥)=−(0,v⃗⊥×n⃗)(0, \vec{n})(0, \vec{v}_{\perp}) = (0, \vec{n} \times \vec{v}_{\perp}) = -(0, \vec{v}_{\perp} \times \vec{n})

Então:

v⃗⊥n⃗=−n⃗v⃗⊥\vec{v}_{\perp} \vec{n} = - \vec{n} \vec{v}_{\perp}

### B. v⃗∥n⃗=n⃗v⃗∥\vec{v}_{\parallel} \vec{n} = \vec{n} \vec{v}_{\parallel}

Como v⃗∥=λn⃗\vec{v}_{\parallel} = \lambda \vec{n}, temos:

v⃗∥n⃗=λn⃗n⃗=λ(−1)=−λ\vec{v}_{\parallel} \vec{n} = \lambda \vec{n} \vec{n} = \lambda (-1) = -\lambda n⃗v⃗∥=λn⃗n⃗=−λ\vec{n} \vec{v}_{\parallel} = \lambda \vec{n} \vec{n} = -\lambda

Logo:

v⃗∥n⃗=n⃗v⃗∥\vec{v}_{\parallel} \vec{n} = \vec{n} \vec{v}_{\parallel}

---

## Passo 2: Calcular V⊥Q−1V_{\perp} Q^{-1} e V∥Q−1V_{\parallel} Q^{-1}

Lembrando que:

Q=cos⁡(θ/2)+sin⁡(θ/2)n⃗(unitaˊrio)Q = \cos(\theta/2) + \sin(\theta/2) \vec{n} \quad \text{(unitário)} Q−1=cos⁡(θ/2)−sin⁡(θ/2)n⃗Q^{-1} = \cos(\theta/2) - \sin(\theta/2) \vec{n}

### A. Produto V⊥Q−1V_{\perp} Q^{-1}

V⊥Q−1=V⊥(cos⁡(θ/2)−sin⁡(θ/2)n⃗)=cos⁡(θ/2)V⊥−sin⁡(θ/2)V⊥n⃗V_{\perp} Q^{-1} = V_{\perp} (\cos(\theta/2) - \sin(\theta/2) \vec{n}) \\ = \cos(\theta/2) V_{\perp} - \sin(\theta/2) V_{\perp} \vec{n}

Usando V⊥n⃗=−n⃗V⊥V_{\perp} \vec{n} = - \vec{n} V_{\perp}:

=cos⁡(θ/2)V⊥+sin⁡(θ/2)n⃗V⊥= \cos(\theta/2) V_{\perp} + \sin(\theta/2) \vec{n} V_{\perp}

Agora, observe que isso é exatamente:

QV⊥=(cos⁡(θ/2)+sin⁡(θ/2)n⃗)V⊥Q V_{\perp} = (\cos(\theta/2) + \sin(\theta/2) \vec{n}) V_{\perp}

Logo:

V⊥Q−1=QV⊥V_{\perp} Q^{-1} = Q V_{\perp}

✔️ **Prova concluída para a parte perpendicular.**

---

### B. Produto V∥Q−1V_{\parallel} Q^{-1}

V∥Q−1=cos⁡(θ/2)V∥−sin⁡(θ/2)V∥n⃗V_{\parallel} Q^{-1} = \cos(\theta/2) V_{\parallel} - \sin(\theta/2) V_{\parallel} \vec{n}

Mas aqui V∥n⃗=n⃗V∥V_{\parallel} \vec{n} = \vec{n} V_{\parallel}, então:

=cos⁡(θ/2)V∥−sin⁡(θ/2)n⃗V∥=(cos⁡(θ/2)−sin⁡(θ/2)n⃗)V∥=Q−1V∥= \cos(\theta/2) V_{\parallel} - \sin(\theta/2) \vec{n} V_{\parallel} = (\cos(\theta/2) - \sin(\theta/2) \vec{n}) V_{\parallel} = Q^{-1} V_{\parallel}

✔️ **Prova concluída para a parte paralela.**

---

## ✅ Conclusão

Provas demonstradas:

- V⊥Q−1=QV⊥V_{\perp} Q^{-1} = Q V_{\perp}
    
- V∥Q−1=Q−1V∥V_{\parallel} Q^{-1} = Q^{-1} V_{\parallel}
    

Usamos:

- Produto de quatérnios puros
    
- Decomposição ortogonal do vetor
    
- Definições de QQ e Q−1Q^{-1}
    

Se quiser, posso te mostrar como isso se encaixa na rotação de vetores com quatérnios.