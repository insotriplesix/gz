import javax.xml.bind.annotation.*;

@XmlRootElement(name = "юл")
@XmlAccessorType(XmlAccessType.FIELD)
public class Data1 implements Data {
    @XmlElement(name = "иннюл")
    public String inn;

    @XmlElement(name = "имяюл")
    public String name;

    @Override
    public String toString() {
        return "Data1{" +
                "inn='" + inn + '\'' +
                ", name='" + name + '\'' +
                '}';
    }
}
