Vamos provar as duas identidades:

- VperpQ−1=QVperpVperp Q^{-1} = Q VperpVperp​Q−1=QVperp​
    
- V∥Q−1=Q−1V∥Vpar Q^{-1} = Q^{-1} VparV∥​Q−1=Q−1V∥​
    

**Dica fornecida:** usar a decomposição vetorial v⃗=v⃗∥+v⃗perp\vec{v} = \vec{v}par + \vec{v}perpv=v∥​+vperp​, onde:

- v⃗∥\vec{v}parv∥​ é **paralelo** ao eixo de rotação n⃗\vec{n}n
    
- v⃗perp\vec{v}perpvperp​ é **ortogonal** a n⃗\vec{n}n
    

## Passo 1: Produto de quatérnios puros

Do slide 40:

(0,u⃗)(0,v⃗)=(−⟨u⃗,v⃗⟩,u⃗×v⃗)(0, \vec{u})(0, \vec{v}) = (-\langle \vec{u}, \vec{v} \rangle, \vec{u} \times \vec{v})(0,u)(0,v)=(−⟨u,v⟩,u×v)

**Mostre que:**

- v⃗perpn⃗=−n⃗v⃗perp\vec{v}perp \vec{n} = -\vec{n} \vec{v}perpvperp​n=−nvperp​
    
- v⃗∥n⃗=n⃗v⃗∥\vec{v}par \vec{n} = \vec{n} \vec{v}parv∥​n=nv∥​
    

### A. v⃗perpn⃗=−n⃗v⃗perp\vec{v}perp \vec{n} = -\vec{n} \vec{v}perpvperp​n=−nvperp​

Como v⃗perp⋅n⃗=0\vec{v}perp \cdot \vec{n} = 0vperp​⋅n=0, o produto de quatérnios puros é:

(0,v⃗perp)(0,n⃗)=(0,v⃗perp×n⃗)(0, \vec{v}perp)(0, \vec{n}) = (0, \vec{v}perp \times \vec{n})(0,vperp​)(0,n)=(0,vperp​×n) (0,n⃗)(0,v⃗perp)=(0,n⃗×v⃗perp)=−(0,v⃗perp×n⃗)(0, \vec{n})(0, \vec{v}perp) = (0, \vec{n} \times \vec{v}perp) = -(0, \vec{v}perp \times \vec{n})(0,n)(0,vperp​)=(0,n×vperp​)=−(0,vperp​×n)

Então:

v⃗perpn⃗=−n⃗v⃗perp\vec{v}perp \vec{n} = - \vec{n} \vec{v}perpvperp​n=−nvperp​

### B. v⃗∥n⃗=n⃗v⃗∥\vec{v}par \vec{n} = \vec{n} \vec{v}parv∥​n=nv∥​

Como v⃗∥=λn⃗\vec{v}par = \lambda \vec{n}v∥​=λn, temos:

v⃗∥n⃗=λn⃗n⃗=λ(−1)=−λ\vec{v}par \vec{n} = \lambda \vec{n} \vec{n} = \lambda (-1) = -\lambdav∥​n=λnn=λ(−1)=−λ n⃗v⃗∥=λn⃗n⃗=−λ\vec{n} \vec{v}par = \lambda \vec{n} \vec{n} = -\lambdanv∥​=λnn=−λ

Logo:

v⃗∥n⃗=n⃗v⃗∥\vec{v}par \vec{n} = \vec{n} \vec{v}parv∥​n=nv∥​

---

## Passo 2: Calcular VperpQ−1Vperp Q^{-1}Vperp​Q−1 e V∥Q−1Vpar Q^{-1}V∥​Q−1

Lembrando que:

Q=cos⁡(θ/2)+sin⁡(θ/2)n⃗(unitaˊrio)Q = \cos(\theta/2) + \sin(\theta/2) \vec{n} \quad \text{(unitário)}Q=cos(θ/2)+sin(θ/2)n(unitaˊrio) Q−1=cos⁡(θ/2)−sin⁡(θ/2)n⃗Q^{-1} = \cos(\theta/2) - \sin(\theta/2) \vec{n}Q−1=cos(θ/2)−sin(θ/2)n

### A. Produto VperpQ−1Vperp Q^{-1}Vperp​Q−1

VperpQ−1=Vperp(cos⁡(θ/2)−sin⁡(θ/2)n⃗)=cos⁡(θ/2)Vperp−sin⁡(θ/2)Vperpn⃗Vperp Q^{-1} = Vperp (\cos(\theta/2) - \sin(\theta/2) \vec{n}) \\ = \cos(\theta/2) Vperp - \sin(\theta/2) Vperp \vec{n}Vperp​Q−1=Vperp​(cos(θ/2)−sin(θ/2)n)=cos(θ/2)Vperp​−sin(θ/2)Vperp​n

Usando Vperpn⃗=−n⃗VperpVperp \vec{n} = - \vec{n} VperpVperp​n=−nVperp​:

=cos⁡(θ/2)Vperp+sin⁡(θ/2)n⃗Vperp= \cos(\theta/2) Vperp + \sin(\theta/2) \vec{n} Vperp=cos(θ/2)Vperp​+sin(θ/2)nVperp​

Agora, observe que isso é exatamente:

QVperp=(cos⁡(θ/2)+sin⁡(θ/2)n⃗)VperpQ Vperp = (\cos(\theta/2) + \sin(\theta/2) \vec{n}) VperpQVperp​=(cos(θ/2)+sin(θ/2)n)Vperp​

Logo:

VperpQ−1=QVperpVperp Q^{-1} = Q VperpVperp​Q−1=QVperp​

✔️ **Prova concluída para a parte perpendicular.**

---

### B. Produto V∥Q−1Vpar Q^{-1}V∥​Q−1

V∥Q−1=cos⁡(θ/2)V∥−sin⁡(θ/2)V∥n⃗Vpar Q^{-1} = \cos(\theta/2) Vpar - \sin(\theta/2) Vpar \vec{n}V∥​Q−1=cos(θ/2)V∥​−sin(θ/2)V∥​n

Mas aqui V∥n⃗=n⃗V∥Vpar \vec{n} = \vec{n} VparV∥​n=nV∥​, então:

=cos⁡(θ/2)V∥−sin⁡(θ/2)n⃗V∥=(cos⁡(θ/2)−sin⁡(θ/2)n⃗)V∥=Q−1V∥= \cos(\theta/2) Vpar - \sin(\theta/2) \vec{n} Vpar = (\cos(\theta/2) - \sin(\theta/2) \vec{n}) Vpar = Q^{-1} Vpar=cos(θ/2)V∥​−sin(θ/2)nV∥​=(cos(θ/2)−sin(θ/2)n)V∥​=Q−1V∥​